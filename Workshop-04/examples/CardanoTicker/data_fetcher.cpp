#include "data_fetcher.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include "config.h"
#include "wifi_manager.h"

namespace {

constexpr unsigned long KOIOS_INTERVAL_MS = 60UL * 1000UL;
constexpr unsigned long PORTFOLIO_INTERVAL_MS = 10UL * 60UL * 1000UL;
constexpr size_t MAX_POLICY_IDS = 10;
constexpr size_t MAX_TOKENS = 20;

float walletBalance = 0.0f;
int tokenCount = 0;
int nftCount = 0;
String policyIds[MAX_POLICY_IDS];
int policyIdCount = 0;
TokenInfo tokens[MAX_TOKENS];

unsigned long lastKoiosFetch = 0;
unsigned long lastPortfolioFetch = 0;

void fetchWalletBalance();
void fetchMinSwapData();
void fetchCexplorerData(const String &policyId);

} // namespace

void initDataFetcher() {
  walletBalance = 0.0f;
  tokenCount = 0;
  nftCount = 0;
  policyIdCount = 0;
  lastKoiosFetch = 0;
  lastPortfolioFetch = 0;
  for (size_t i = 0; i < MAX_TOKENS; ++i) {
    tokens[i].ticker = "";
    tokens[i].amount = 0.0f;
    tokens[i].value = 0.0f;
    tokens[i].change24h = 0.0f;
  }
}

void updateKoiosData() {
  if (!wifiManagerIsConnected()) {
    return;
  }

  const unsigned long now = millis();
  // Allow fetch if never fetched before (lastKoiosFetch == 0) or interval has
  // passed
  if (lastKoiosFetch != 0 && (now - lastKoiosFetch) < KOIOS_INTERVAL_MS) {
    return;
  }

  lastKoiosFetch = now;
  fetchWalletBalance();
}

void updatePortfolioData() {
  if (!wifiManagerIsConnected()) {
    return;
  }

  const unsigned long now = millis();
  // Allow fetch if never fetched before (lastPortfolioFetch == 0) or interval
  // has passed
  if (lastPortfolioFetch != 0 &&
      (now - lastPortfolioFetch) < PORTFOLIO_INTERVAL_MS) {
    return;
  }

  lastPortfolioFetch = now;
  fetchMinSwapData();

  for (int i = 0; i < policyIdCount; ++i) {
    fetchCexplorerData(policyIds[i]);
  }
}

float getWalletBalance() { return walletBalance; }

int getTokenCount() { return tokenCount; }

int getNftCount() { return nftCount; }

unsigned long getLastKoiosFetchTime() { return lastKoiosFetch; }

TokenInfo getToken(int index) {
  TokenInfo empty = {"", 0.0f, 0.0f, 0.0f};
  if (index < 0 || index >= tokenCount ||
      index >= static_cast<int>(MAX_TOKENS)) {
    return empty;
  }
  return tokens[index];
}

namespace {

void fetchWalletBalance() {
  Serial.println();
  Serial.println("--- Fetching Wallet Balance from Koios ---");

  HTTPClient http;
  http.begin(koiosApiUrl);
  http.addHeader("Content-Type", "application/json");

  String jsonPayload = "{\"_stake_addresses\":[\"";
  jsonPayload += stakeAddress;
  jsonPayload += "\"]}";

  Serial.println("Sending POST request to Koios...");
  Serial.print("Payload: ");
  Serial.println(jsonPayload);

  int httpResponseCode = http.POST(jsonPayload);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, response);

    if (!error) {
      if (doc.is<JsonArray>() && doc.size() > 0) {
        JsonObject accountInfo = doc[0];
        const char *balanceStr = accountInfo["total_balance"];
        long long balanceLovelace = 0;
        if (balanceStr != nullptr) {
          balanceLovelace = atoll(balanceStr);
        }
        walletBalance = balanceLovelace / 1000000.0;

        Serial.println();
        Serial.println("✓ Wallet Balance Fetched Successfully!");
        Serial.print("Stake Address: ");
        Serial.println(accountInfo["stake_address"].as<const char *>());
        Serial.print("Total Balance: ");
        Serial.print(walletBalance, 6);
        Serial.println(" ADA");
      } else {
        Serial.println("Error: Empty response from Koios API");
      }
    } else {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("Error in HTTP request. Response Code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void fetchMinSwapData() {
  Serial.println();
  Serial.println("--- Fetching Tokens and NFTs from MinSwap ---");

  HTTPClient http;
  String fullUrl = String(minswapApiUrl);
  fullUrl += "?address=";
  fullUrl += walletAddress;
  fullUrl += "&only_minswap=true&filter_small_value=false";

  Serial.print("Requesting: ");
  Serial.println(fullUrl);

  http.begin(fullUrl);
  Serial.println("Sending GET request to MinSwap...");
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, response);

    if (!error) {
      Serial.println();
      Serial.println("✓ MinSwap Data Fetched Successfully!");

      if (doc.containsKey("positions")) {
        JsonObject positions = doc["positions"];

        if (positions.containsKey("nft_positions")) {
          JsonArray nftArray = positions["nft_positions"];
          nftCount = nftArray.size();
          Serial.print("NFTs found: ");
          Serial.println(nftCount);

          policyIdCount = 0;
          for (int i = 0;
               i < nftArray.size() && i < static_cast<int>(MAX_POLICY_IDS);
               ++i) {
            JsonObject nft = nftArray[i];
            const char *currencySymbol = nft["currency_symbol"];
            if (currencySymbol != nullptr) {
              policyIds[policyIdCount] = String(currencySymbol);
              ++policyIdCount;

              Serial.print("  NFT ");
              Serial.print(i + 1);
              Serial.print(": Policy ID (currency_symbol): ");
              Serial.println(currencySymbol);
            }
          }

          Serial.print("\nExtracted ");
          Serial.print(policyIdCount);
          Serial.println(" policy ID(s) for Cexplorer API calls");
        }

        if (positions.containsKey("asset_positions")) {
          JsonArray assetArray = positions["asset_positions"];
          tokenCount = assetArray.size();
          if (tokenCount > static_cast<int>(MAX_TOKENS)) {
            tokenCount = MAX_TOKENS;
          }
          Serial.print("Tokens found: ");
          Serial.println(tokenCount);

          for (int i = 0;
               i < assetArray.size() && i < static_cast<int>(MAX_TOKENS); ++i) {
            JsonObject asset = assetArray[i];
            if (asset.containsKey("asset")) {
              JsonObject assetInfo = asset["asset"];
              if (assetInfo.containsKey("metadata")) {
                JsonObject metadata = assetInfo["metadata"];
                String ticker = metadata["ticker"] | "UNKNOWN";
                String name = metadata["name"] | "Unknown Token";
                float priceUsd = asset["price_usd"] | 0.0f;
                float amount = asset["amount"] | 0.0f;
                float change24h = asset["pnl_24h_percent"] | 0.0f;

                // Store token data
                tokens[i].ticker = ticker;
                tokens[i].amount = amount;
                tokens[i].value = priceUsd * amount; // Calculate total value
                tokens[i].change24h = change24h;

                Serial.print("  Token ");
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(ticker);
                Serial.print(" (");
                Serial.print(name);
                Serial.print(") - Price: $");
                Serial.print(priceUsd, 4);
                Serial.print(", Amount: ");
                Serial.print(amount, 2);
                Serial.print(", 24h Change: ");
                Serial.print(change24h, 2);
                Serial.println("%");
              }
            }
          }
        }
      } else {
        Serial.println("Warning: No positions found in MinSwap response");
      }
    } else {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("Error in HTTP request. Response Code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void fetchCexplorerData(const String &policyId) {
  Serial.println();
  Serial.println("--- Fetching NFT Info from Cexplorer ---");
  Serial.print("Policy ID: ");
  Serial.println(policyId);

  HTTPClient http;
  String fullUrl = String(cexplorerApiUrl);
  fullUrl += "?id=";
  fullUrl += policyId;

  Serial.print("Requesting: ");
  Serial.println(fullUrl);

  http.begin(fullUrl);
  // http.addHeader("api-key", cexplorerApiKey);

  Serial.println("Sending GET request to Cexplorer...");
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, response);

    if (!error) {
      Serial.println();
      Serial.println("✓ Cexplorer Data Fetched Successfully!");

      if (doc.containsKey("data")) {
        JsonObject data = doc["data"];
        if (data.containsKey("collection")) {
          JsonObject collection = data["collection"];
          String collectionName = collection["name"] | "Unknown";

          Serial.print("Collection Name: ");
          Serial.println(collectionName);

          if (collection.containsKey("stats")) {
            JsonObject stats = collection["stats"];
            long floorLovelace = stats["floor"] | 0;
            float floorPriceAda = floorLovelace / 1000000.0f;
            int owners = stats["owners"] | 0;

            Serial.print("Floor Price: ");
            Serial.print(floorPriceAda, 2);
            Serial.println(" ADA");
            Serial.print("Owners: ");
            Serial.println(owners);
          }
        }
      } else {
        Serial.println("Warning: No data found in Cexplorer response");
      }
    } else {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("Error in HTTP request. Response Code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 401) {
      Serial.println("Error: 401 Unauthorized - Check your Cexplorer API key!");
    }
  }

  http.end();
}

} // namespace
