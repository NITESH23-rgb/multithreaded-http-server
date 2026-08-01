// script.js

// 🌙 Toggle Dark Mode
const toggleDarkMode = () => {
  document.body.classList.toggle('dark-mode');
};

// 🖱️ Add event listener for dark mode button
document.addEventListener('DOMContentLoaded', () => {
  const btn = document.createElement('button');
  btn.textContent = 'Toggle Dark Mode';
  btn.style.position = 'fixed';
  btn.style.bottom = '20px';
  btn.style.right = '20px';
  btn.style.padding = '10px 15px';
  btn.style.background = '#333';
  btn.style.color = '#fff';
  btn.style.border = 'none';
  btn.style.borderRadius = '6px';
  btn.style.cursor = 'pointer';
  btn.style.zIndex = '1000';
  btn.addEventListener('click', toggleDarkMode);
  document.body.appendChild(btn);

  // ✨ Welcome animation
  const header = document.querySelector('header h1');
  if (header) {
    header.style.opacity = '0';
    header.style.transition = 'opacity 1.5s ease-in-out';
    setTimeout(() => {
      header.style.opacity = '1';
    }, 500);
  }

  // ✅ Form validation setup
  const loginForm = document.querySelector('form[action="/login"]');
  const registerForm = document.querySelector('form[action="/register"]');

  // 🔐 Login form validation
  if (loginForm) {
    loginForm.addEventListener('submit', (e) => {
      const username = loginForm.querySelector('#username').value.trim();
      const password = loginForm.querySelector('#password').value.trim();

      if (username === '' || password === '') {
        e.preventDefault();
        alert('Please fill in both username and password.');
      } else if (password.length < 6) {
        e.preventDefault();
        alert('Password must be at least 6 characters long.');
      }
    });
  }

  // 📝 Registration form validation
  if (registerForm) {
    registerForm.addEventListener('submit', (e) => {
      const username = registerForm.querySelector('#username').value.trim();
      const email = registerForm.querySelector('#email').value.trim();
      const password = registerForm.querySelector('#password').value.trim();
      const confirmPassword = registerForm.querySelector('#confirmPassword').value.trim();

      if (!username || !email || !password || !confirmPassword) {
        e.preventDefault();
        alert('All fields are required.');
      } else if (!email.includes('@')) {
        e.preventDefault();
        alert('Please enter a valid email address.');
      } else if (password.length < 6) {
        e.preventDefault();
        alert('Password must be at least 6 characters long.');
      } else if (password !== confirmPassword) {
        e.preventDefault();
        alert('Passwords do not match.');
      }
    });
  }
});

// 🌗 Dark mode styling
const style = document.createElement('style');
style.textContent = `
  .dark-mode {
    background: #121212;
    color: #f5f5f5;
  }
  .dark-mode header {
    background: rgba(255,255,255,0.1);
  }
  .dark-mode footer {
    background: rgba(255,255,255,0.1);
  }
`;
document.head.appendChild(style);
