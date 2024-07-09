import React from 'react';
import logo from './logo.svg';
import './App.css';

function App() {
  return (
    <div className="App">
      <header className="App-header">      
        <h1>Welcome to Evertz Garden</h1>
      </header>
      <nav className="App-nav">
        <ul>
          <li><a href="#home">Home</a></li>
          <li><a href="#about">About</a></li>
          <li><a href="#Gallery">gallery</a></li>
          <li><a href="#contact">Contact</a></li>
        </ul>
      </nav>
      <main className="App-main">
        <section>
          <p>Weather forecast</p>
        </section>
      </main>
      <footer className="App-footer">
        <p>&copy; 2024 Evertz Garden. All rights reserved.</p>
      </footer>
      
    </div>
  );
}

export default App;
