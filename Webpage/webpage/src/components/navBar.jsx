import React from "react";
import { Link } from "react-router-dom";

import '../styles/style.css'

const NavBar = () => {
    return (
        <div className="App">
            <header className="App-header">
                <h1>Welcome to Evertz Garden</h1>
            </header>
            <nav className="App-nav">
                <ul>
                    <li><Link to="/">Home</Link></li>
                    <li><Link to="/about">About</Link></li>
                    <li><Link to="/gallery">Gallery</Link></li>
                </ul>
            </nav>
        </div>
    )
}

export default NavBar;