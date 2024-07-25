import React, { useState, useEffect } from 'react';
import axios from 'axios';
import '../styles/style.css';

const Weather = () => {
    const [rainData, setRainData] = useState([]);
    const [isWatering, setIsWatering] = useState(false);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);

    useEffect(() => {
        const fetchWeatherData = async () => {
            try {
                const response = await axios.get('https://api.open-meteo.com/v1/forecast?latitude=43.39141047955725&longitude=-79.76961512140457&hourly=precipitation_probability&timezone=auto&past_days=1&forecast_days=1');
                const data = response.data;
                const times = data.hourly.time;
                const precipitation = data.hourly.precipitation_probability;
                const today = new Date().toISOString().split('T')[0];
                const rainData = times.map((time, index) => ({
                    time,
                    precipitation: precipitation[index]
                })).filter(item => item.time.startsWith(today));

                setRainData(rainData);
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

        var axios = require('axios');
        var data = '1ZONE|2HOUR';

        var config = {
            method: 'post',
            url: '172.20.10.8/esp_rx',
            headers: {},
            data: data
        };

        axios(config)
            .then(function (response) {
                console.log(JSON.stringify(response.data));
            })
            .catch(function (error) {
                console.log(error);
            });

        // try {
        //     await axios.post('http://172.20.10.8/uri/post', {
        //         'data': 'hey'
        //     }, {
        //         headers: {
        //             'Content-Type': 'application/x-www-form-urlencoded',
        //         }
        //     }).then((response) => {
        //         console.log(response.data);
        //         setIsWatering(false);
        //     });
        // } catch (error) {
        //     console.error('Error watering plants:', error);
        //     setIsWatering(false);
        // }
    };


    if (loading) {
        return <div>Loading...</div>;
    }

    if (error) {
        return <div>Error fetching data: {error.message}</div>;
    }

    return (
        <div className="weather-container">
            <h1>Today's Rain Forecast</h1>
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