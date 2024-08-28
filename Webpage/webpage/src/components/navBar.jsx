import React from "react";
import { Link } from "react-router-dom";
import logo from '../images/logo.png';
import '../styles/style.css';

const NavBar = () => {
    return (
        <div className="App">
            <header className="App-header">
                <img src={logo} alt="Logo" className="nav-logo" />
                <h1>Welcome to Evertz Garden</h1>
                <div className="nav-container">
                    <nav className="App-nav">
                        <ul>
                            <li><Link to="/">Home</Link></li>
                            <li><Link to="/about">About</Link></li>
                            <li><Link to="/gallery">Gallery</Link></li>
                        </ul>
                    </nav>
                </div>
            </header>
        </div>
    );
}

export default NavBar;