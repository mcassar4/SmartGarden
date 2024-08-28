import React from 'react'
import '../styles/style.css';
import logo from '../images/logo.png';

const Footer = () => {
    return (
        <footer className="footer">
            <img src={logo} alt="Logo" className="footer-logo" />
            <p>&copy; 2024 Evertz Garden. All rights reserved.</p>
        </footer>
    );
}

export default Footer;