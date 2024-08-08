import React, { useState, useEffect } from 'react';
import axios from 'axios';
import '../styles/style.css';

const generateHours = () => {
    const hours = ['Now'];
    for (let i = 0; i < 24; i++) {
        const hour = i % 12 === 0 ? 12 : i % 12;
        const period = i < 12 ? 'AM' : 'PM';
        hours.push(`${hour}:00 ${period}`);
    }
    return hours;
};

const getCurrentHourPlusOne = () => {
    const date = new Date();
    let nextHour = date.getHours() + 1;
    if (nextHour >= 24) nextHour = 0;
    const hour = nextHour % 12 === 0 ? 12 : nextHour % 12;
    const period = nextHour < 12 ? 'AM' : 'PM';
    return `${hour}:00 ${period}`;
};

const Weather = () => {
    const [rainData, setRainData] = useState([]);
    const [isWatering, setIsWatering] = useState(false);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);
    const [currentWeather, setCurrentWeather] = useState(null);
    const [currentPrecipitation, setCurrentPrecipitation] = useState(null);
    const [selectedZone, setSelectedZone] = useState('All');
    const [selectedHour, setSelectedHour] = useState(getCurrentHourPlusOne());

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

                const currentHour = new Date().getHours();
                const currentPrecipitation = rainData.find(item => new Date(item.time).getHours() === currentHour);
                setCurrentPrecipitation(currentPrecipitation ? currentPrecipitation.precipitation : null);

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

        try {
            for (let i = 1; i < 6; i++) {
                var min = 70;

                if (i === 2) {
                    min = 80;
                } else {
                    min = 70;
                }

                var data = i + ':' + min;

                await axios.post('http://172.20.10.11/esp_rx', data).then((response) => {
                    console.log(response.data);
                    if (response.data !== 'rec') {
                        i--;
                        console.log('Not going to ESP');
                    }
                    setIsWatering(false);
                });
            }
        } catch (error) {
            console.error('Error watering plants:', error);
            setIsWatering(false);
        }

        try {
            axios.get('http://172.20.10.11/esp_tx').then((response) => {
                console.log(JSON.stringify(response.data['key1']));
                setIsWatering(false);
            });
        } catch (error) {
            console.error('Error watering plants:', error);
            setIsWatering(false);
        }
    };

    const zones = ['1', '2', '3', '4', '5', 'All'];
    const hours = generateHours();
    const defaultZone = 'All';

    const handleZoneSelect = (e) => {
        setSelectedZone(e.target.value);
    };

    const handleHourSelect = (e) => {
        setSelectedHour(e.target.value);
    };

    if (loading) {
        return <div className="loading-spinner">Loading...</div>;
    }

    if (error) {
        return <div className="error-message">Error fetching data: {error.message}</div>;
    }

    return (
        <div className="weather-container">
            {currentWeather && (
                <div className="current-weather-container">
                    <h2>Current Weather</h2>
                    <div>Temperature: {currentWeather.temperature}°C</div>
                    {currentPrecipitation !== null && (
                        <div>Precipitation Probability: {currentPrecipitation}%</div>
                    )}
                </div>
            )}
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
            <div>
                <h2>Manual Water Plant Control</h2>
                <div className="dropdown-container">
                    <div>
                        <label htmlFor="zone-select">Select Zone:</label>
                        <select id="zone-select" defaultValue={defaultZone} onChange={handleZoneSelect}>
                            <option value="" disabled>Select a zone</option>
                            {zones.map((zone, index) => (
                                <option key={index} value={zone}>
                                    {zone}
                                </option>
                            ))}
                        </select>
                    </div>
                    <div>
                        <label htmlFor="hour-select">Select Hour:</label>
                        <select id="hour-select" defaultValue={selectedHour} onChange={handleHourSelect}>
                            <option value="" disabled>Select an hour</option>
                            {hours.map((hour, index) => (
                                <option key={index} value={hour}>
                                    {hour}
                                </option>
                            ))}
                        </select>
                    </div>
                    <button onClick={handleWaterPlants} className="water-button">
                        {isWatering ? 'Watering...' : 'Water Plants'}
                    </button>
                </div>
            </div>
        </div>
    );
};

export default Weather;
