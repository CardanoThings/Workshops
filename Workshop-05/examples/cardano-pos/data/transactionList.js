const transactionsContainer = document.getElementById('transactionsContainer');

// Format timestamp to readable date
function formatTimestamp(timestamp) {
    const date = new Date(timestamp);
    return date.toLocaleString();
}

// Load and display transactions
async function loadTransactions() {
    try {
        const response = await fetch('/api/transactions');
        if (!response.ok) {
            throw new Error('Failed to fetch transactions');
        }

        const transactions = await response.json();
        displayTransactions(transactions);
    } catch (error) {
        console.error('Error loading transactions:', error);
        transactionsContainer.innerHTML = '<p>Error loading transactions</p>';
    }
}

// Display transactions in table
function displayTransactions(transactions) {
    if (transactions.length === 0) {
        transactionsContainer.innerHTML = '<p>No transactions yet</p>';
        return;
    }

    const table = document.createElement('table');
    table.className = 'transactions-table';

    // Create header
    const thead = document.createElement('thead');
    const headerRow = document.createElement('tr');
    ['ID', 'Amount (ADA)', 'Timestamp', 'Transaction Hash'].forEach(header => {
        const th = document.createElement('th');
        th.textContent = header;
        headerRow.appendChild(th);
    });
    thead.appendChild(headerRow);
    table.appendChild(thead);

    // Create body
    const tbody = document.createElement('tbody');
    transactions.forEach(transaction => {
        const row = document.createElement('tr');

        const idCell = document.createElement('td');
        idCell.textContent = transaction.id;
        row.appendChild(idCell);

        const amountCell = document.createElement('td');
        // Convert lovelace to ADA for display (1 ADA = 1,000,000 lovelace)
        const adaAmount = (transaction.amount / 1000000).toFixed(2);
        amountCell.textContent = adaAmount;
        row.appendChild(amountCell);

        const timestampCell = document.createElement('td');
        timestampCell.textContent = formatTimestamp(transaction.timestamp);
        row.appendChild(timestampCell);

        const hashCell = document.createElement('td');
        hashCell.textContent = transaction.txHash || '-';
        hashCell.className = transaction.txHash ? '' : 'empty-hash';
        row.appendChild(hashCell);

        tbody.appendChild(row);
    });
    table.appendChild(tbody);

    transactionsContainer.innerHTML = '';
    transactionsContainer.appendChild(table);
}

// Load transactions on page load
loadTransactions();

// Refresh transactions after successful payment creation
// This will be called from requestPayment.js after a successful transaction
window.refreshTransactions = loadTransactions;

