import React from 'react'

import NavBar from "../components/navBar";
import Footer from "../components/footer";
import Weather from "./weather";

const Home = () => {
    return (
        <div className="App">
            <NavBar />

            <div className="weather-root">
                <Weather />
            </div>

            <div className='empty-height'></div>
            <div className='empty-height'></div>

            <Footer />
        </div>
    );
};

export default Home;