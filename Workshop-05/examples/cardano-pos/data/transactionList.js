/**
 * Transaction List Handler
 * 
 * This module handles fetching and displaying transactions from the API.
 * It creates a table view of all transactions, automatically refreshes every
 * 30 seconds, and provides a manual refresh function for other modules.
 */

// Get reference to the container element where transactions will be displayed
const transactionsContainer = document.getElementById('transactionsContainer');

/**
 * Format timestamp to readable date string
 * Converts Unix timestamp (milliseconds) to localized date/time string
 * 
 * @param {number} timestamp - Unix timestamp in milliseconds
 * @returns {string} Formatted date/time string (e.g., "12/25/2023, 3:45:30 PM")
 */
function formatTimestamp(timestamp) {
    const date = new Date(timestamp);
    return date.toLocaleString();
}

/**
 * Load transactions from API and display them
 * Fetches all transactions from the GET /api/transactions endpoint
 * and calls displayTransactions() to render them
 */
async function loadTransactions() {
    try {
        // Fetch transactions from API
        const response = await fetch('/api/transactions');

        // Check if request was successful
        if (!response.ok) {
            throw new Error('Failed to fetch transactions');
        }

        // Parse JSON response
        const transactions = await response.json();

        // Display transactions in the table
        displayTransactions(transactions);
    } catch (error) {
        // Log error and show error message in container
        console.error('Error loading transactions:', error);
        transactionsContainer.innerHTML = '<p>Error loading transactions</p>';
    }
}

/**
 * Display transactions in a table format
 * Creates a table with columns: ID, Amount (ADA), Timestamp, Transaction Hash
 * 
 * @param {Array} transactions - Array of transaction objects from API
 */
function displayTransactions(transactions) {
    // Handle empty transaction list
    if (transactions.length === 0) {
        transactionsContainer.innerHTML = '<p>No transactions yet</p>';
        return;
    }

    // Create table element
    const table = document.createElement('table');
    table.className = 'transactions-table';

    // Create table header
    const thead = document.createElement('thead');
    const headerRow = document.createElement('tr');

    // Create header cells for each column
    ['ID', 'Amount (ADA)', 'Timestamp', 'Transaction Hash'].forEach(header => {
        const th = document.createElement('th');
        th.textContent = header;
        headerRow.appendChild(th);
    });
    thead.appendChild(headerRow);
    table.appendChild(thead);

    // Create table body
    const tbody = document.createElement('tbody');

    // Create a row for each transaction
    transactions.forEach(transaction => {
        const row = document.createElement('tr');

        // Transaction ID cell
        const idCell = document.createElement('td');
        idCell.textContent = transaction.id;
        row.appendChild(idCell);

        // Amount cell - convert lovelace to ADA for display
        const amountCell = document.createElement('td');
        // Note: transaction.amount includes the transaction ID, so this displays
        // the full amount including ID. For display purposes, we show it as ADA.
        // 1 ADA = 1,000,000 lovelace
        const adaAmount = (transaction.amount / 1000000).toFixed(2);
        amountCell.textContent = adaAmount;
        row.appendChild(amountCell);

        // Timestamp cell - format to readable date
        const timestampCell = document.createElement('td');
        timestampCell.textContent = formatTimestamp(transaction.timestamp);
        row.appendChild(timestampCell);

        // Transaction hash cell
        const hashCell = document.createElement('td');
        // Show "-" if hash is empty (payment not yet confirmed)
        hashCell.textContent = transaction.txHash || '-';
        // Add CSS class for styling empty hashes (grayed out)
        hashCell.className = transaction.txHash ? '' : 'empty-hash';
        row.appendChild(hashCell);

        // Add row to table body
        tbody.appendChild(row);
    });
    table.appendChild(tbody);

    // Clear container and add the new table
    transactionsContainer.innerHTML = '';
    transactionsContainer.appendChild(table);
}

// Polling interval ID (stored so it can be cleared if needed)
let pollingInterval = null;

/**
 * Start automatic polling for transactions
 * Fetches transactions from API every 30 seconds to keep the list up-to-date
 * This ensures new transactions and updated hashes are displayed automatically
 */
function startTransactionPolling() {
    // Clear any existing interval to prevent multiple polling instances
    if (pollingInterval) {
        clearInterval(pollingInterval);
    }

    // Set up interval to fetch transactions every 30 seconds
    pollingInterval = setInterval(() => {
        console.log('Polling for transactions...');
        loadTransactions();
    }, 30000); // 30 seconds = 30000 milliseconds
}

/**
 * Stop automatic polling
 * Useful for cleanup or if polling needs to be disabled
 */
function stopTransactionPolling() {
    if (pollingInterval) {
        clearInterval(pollingInterval);
        pollingInterval = null;
    }
}

// Load transactions immediately when page loads
loadTransactions();

// Start automatic polling to keep transaction list updated
startTransactionPolling();

/**
 * Expose refresh function globally for other modules
 * This allows requestPayment.js to manually refresh the transaction list
 * after creating a new payment request, providing immediate feedback
 */
window.refreshTransactions = loadTransactions;

