// Cardano POS Payment Request Handler

const API_BASE = '/api';
const KOIOS_BASE = 'https://preprod.koios.rest/api/v1/'

// Handle form submission
document.getElementById('paymentForm').addEventListener('submit', async (e) => {
    e.preventDefault();

    const amountInput = document.getElementById('adaAmount');
    const amount = parseFloat(amountInput.value);
    const messageDiv = document.getElementById('message');
    const requestBtn = document.getElementById('requestBtn');

    if (isNaN(amount) || amount <= 0) {
        showMessage('Please enter a valid ADA amount', 'error');
        return;
    }

    // Disable button during request
    requestBtn.disabled = true;
    requestBtn.textContent = 'Processing...';
    messageDiv.textContent = '';

    try {
        // Convert ADA to lovelace (1 ADA = 1,000,000 lovelace)
        const lovelace = Math.floor(amount * 1000000);

        const response = await fetch(`${API_BASE}/transactions`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                amount: lovelace,
                created_at: new Date().toISOString()
            }),
        });

        if (response.ok) {
            const data = await response.json();
            showMessage(`Payment request created successfully! Transaction ID: ${data.id}`, 'success');
            amountInput.value = '';
        } else {
            const error = await response.text();
            showMessage(`Error: ${error}`, 'error');
        }
    } catch (error) {
        showMessage(`Network error: ${error.message}`, 'error');
    } finally {
        requestBtn.disabled = false;
        requestBtn.textContent = 'Request Payment';
    }
});

function showMessage(message, type) {
    const messageDiv = document.getElementById('message');
    messageDiv.textContent = message;
    messageDiv.className = `message ${type}`;

    // Clear message after 5 seconds
    setTimeout(() => {
        messageDiv.textContent = '';
        messageDiv.className = '';
    }, 5000);
}

