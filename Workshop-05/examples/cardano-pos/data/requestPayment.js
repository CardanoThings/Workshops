/**
 * Payment Request Handler
 * 
 * This module handles the creation of new payment requests through a modal dialog.
 * It converts ADA amounts to lovelace, sends POST requests to the API, and
 * triggers the transaction list refresh.
 */

// Get references to DOM elements
const modal = document.getElementById('paymentModal');
const openBtn = document.getElementById('openPaymentModal');
const cancelBtn = document.getElementById('cancelBtn');
const form = document.getElementById('paymentForm');
const amountInput = document.getElementById('adaAmount');
const submitBtn = form.querySelector('button[type="submit"]');

// Open modal when "New Payment Request" button is clicked
openBtn.addEventListener('click', () => {
    modal.showModal(); // Show the native <dialog> element
    amountInput.focus(); // Automatically focus the amount input field
});

// Close modal when cancel button is clicked
cancelBtn.addEventListener('click', () => modal.close());

// Reset form when dialog closes (handles both cancel and successful submission)
modal.addEventListener('close', () => form.reset());

// Handle form submission - creates a new payment request
form.addEventListener('submit', async (e) => {
    e.preventDefault(); // Prevent default form submission behavior

    // Validate and parse the ADA amount input
    const adaAmount = parseFloat(amountInput.value);
    if (isNaN(adaAmount) || adaAmount <= 0) {
        console.log('Please enter a valid ADA amount greater than 0');
        return; // Exit early if validation fails
    }

    // Convert ADA to lovelace (1 ADA = 1,000,000 lovelace)
    // Math.round() ensures we get an integer value
    const lovelaceAmount = Math.round(adaAmount * 1000000);

    // Get current timestamp in milliseconds (Unix timestamp)
    const timestamp = Date.now();

    // Disable submit button and show processing state
    submitBtn.disabled = true;
    submitBtn.textContent = 'Processing...';

    try {
        // Prepare request data with amount in lovelace and timestamp
        const requestData = { amount: lovelaceAmount, timestamp: timestamp };
        console.log('Sending request:', requestData);

        // Send POST request to create new transaction
        const response = await fetch('/api/transactions', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(requestData)
        });

        // Check if request was successful
        if (!response.ok) {
            // Parse error response and throw error
            const error = await response.json();
            throw new Error(error.error || 'Failed to create payment request');
        }

        // Parse successful response
        const transaction = await response.json();

        // Close modal after successful creation
        modal.close();

        // Convert lovelace back to ADA for display in console log
        // Note: transaction.amount includes the transaction ID, so this is for display only
        const adaDisplay = (transaction.amount / 1000000).toFixed(6);
        console.log(`Payment request created! ID: ${transaction.id}, Amount: ${adaDisplay} ADA (${transaction.amount} lovelace)`);

        // Refresh transaction list to show the new transaction
        // window.refreshTransactions is defined in transactionList.js
        if (window.refreshTransactions) {
            window.refreshTransactions();
        }
    } catch (error) {
        // Log error to console (no alert shown to user)
        console.error('Error creating payment request:', error.message);
    } finally {
        // Always re-enable submit button and reset text, even if request failed
        submitBtn.disabled = false;
        submitBtn.textContent = 'Create Payment Request';
    }
});

