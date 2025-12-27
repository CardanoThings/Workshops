const modal = document.getElementById('paymentModal');
const openBtn = document.getElementById('openPaymentModal');
const cancelBtn = document.getElementById('cancelBtn');
const form = document.getElementById('paymentForm');
const amountInput = document.getElementById('adaAmount');
const submitBtn = form.querySelector('button[type="submit"]');

// Open modal
openBtn.addEventListener('click', () => {
    modal.showModal();
    amountInput.focus();
});

// Close modal
cancelBtn.addEventListener('click', () => modal.close());

// Reset form when dialog closes
modal.addEventListener('close', () => form.reset());

// Handle form submission
form.addEventListener('submit', async (e) => {
    e.preventDefault();

    const adaAmount = parseFloat(amountInput.value);
    if (isNaN(adaAmount) || adaAmount <= 0) {
        console.log('Please enter a valid ADA amount greater than 0');
        return;
    }

    // Convert ADA to lovelace (1 ADA = 1,000,000 lovelace)
    const lovelaceAmount = Math.round(adaAmount * 1000000);
    const timestamp = Date.now();

    submitBtn.disabled = true;
    submitBtn.textContent = 'Processing...';

    try {
        const requestData = { amount: lovelaceAmount, timestamp: timestamp };
        console.log('Sending request:', requestData);

        const response = await fetch('/api/transactions', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(requestData)
        });

        if (!response.ok) {
            const error = await response.json();
            throw new Error(error.error || 'Failed to create payment request');
        }

        const transaction = await response.json();
        modal.close();
        // Convert lovelace back to ADA for display
        const adaDisplay = (transaction.amount / 1000000).toFixed(6);
        console.log(`Payment request created! ID: ${transaction.id}, Amount: ${adaDisplay} ADA (${transaction.amount} lovelace)`);

        // Refresh transaction list
        if (window.refreshTransactions) {
            window.refreshTransactions();
        }
    } catch (error) {
        console.error('Error creating payment request:', error.message);
    } finally {
        submitBtn.disabled = false;
        submitBtn.textContent = 'Create Payment Request';
    }
});

