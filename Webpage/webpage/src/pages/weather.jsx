import React, { useState, useEffect } from 'react';
import axios from 'axios';
import '../styles/style.css';

const Weather = () => {
    const [rainData, setRainData] = useState([]);
    const [isWatering, setIsWatering] = useState(false);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);
    const [currentWeather, setCurrentWeather] = useState(null);

    const latitude = 43.39141047955725;
    const longitude = -79.76961512140457;

    useEffect(() => {
        const fetchWeatherData = async () => {
            try {
                setLoading(true);
                const response = await axios.get(`https://api.open-meteo.com/v1/forecast?latitude=${latitude}&longitude=${longitude}&hourly=precipitation_probability,temperature_2m&current_weather=true&timezone=auto&past_days=1&forecast_days=1`);
                const data = response.data;
                const times = data.hourly.time;
                const precipitation = data.hourly.precipitation_probability;
                const today = new Date().toISOString().split('T')[0];
                const rainData = times.map((time, index) => ({
                    time,
                    precipitation: precipitation[index]
                })).filter(item => item.time.startsWith(today));

                setRainData(rainData);
                setCurrentWeather(data.current_weather);
                setLoading(false);
            } catch (error) {
                setError(error);
                setLoading(false);
            }
        };

        fetchWeatherData();
    }, []);

    const handleWaterPlants = async () => {
        setIsWatering(true);

        const data = {
            zone: '1ZONE',
            duration: '2HOUR'
        };

        try {
            const response = await axios.post('http://172.20.10.8/esp_rx', data, {
                headers: {
                    'Content-Type': 'application/json',
                }
            });
            console.log(JSON.stringify(response.data));
            setIsWatering(false);
        } catch (error) {
            console.error('Error watering plants:', error);
            setIsWatering(false);
        } finally {
            setIsWatering(false);
        }
    };

    if (loading) {
        return <div className="loading-spinner">Loading...</div>;
    }

    if (error) {
        return <div className="error-message">Error fetching data: {error.message}</div>;
    }

    return (
        <div className="weather-container">
            <h2>Today's Rain Forecast</h2>
            {/* {currentWeather && (
                <div className="current-weather-container">
                    <h3>Current Weather</h3>
                    <div>Temperature: {currentWeather.temperature}°C</div>
                    <div>Condition: {currentWeather.weathercode}</div>
                </div> */}
            {/* )} */}
            <div className="rain-data-container">
                {rainData.length > 0 ? (
                    rainData.map((item, index) => (
                        <div key={index} className="rain-data-item">
                            <div>{new Date(item.time).getHours()}:00</div>
                            <div>{item.precipitation}%</div>
                        </div>
                    ))
                ) : (
                    <div>No rain expected today.</div>
                )}
            </div>
            <button onClick={handleWaterPlants} className="water-button">
                {isWatering ? 'Watering...' : 'Water Plants'}
            </button>
        </div>
    );
};

export default Weather;