import React, { useState, useEffect, useCallback } from 'react';
import axios from 'axios';
import '../styles/style.css';

const API_BASE_URL = 'http://localhost:5000'; // Update with your server URL

const durations = [5, 10, 15, 30, 60, 120, 180, 240];

const generateTimeSlots = () => {
    const timeSlots = [];
    const now = new Date();

    const roundedMinutes = Math.ceil(now.getMinutes() / 15) * 15;
    const nextSlot = new Date(now);
    nextSlot.setMinutes(roundedMinutes);
    nextSlot.setSeconds(0);
    nextSlot.setMilliseconds(0);

    timeSlots.push({ label: 'Now', value: now });

    for (let i = 0; i < 48; i++) {  
        const futureTime = new Date(nextSlot.getTime() + i * 15 * 60 * 1000);
        if (futureTime.getTime() <= now.getTime()) {
            continue;  
        }
        const hours = futureTime.getHours() % 12 === 0 ? 12 : futureTime.getHours() % 12;
        const minutes = futureTime.getMinutes().toString().padStart(2, '0');
        const period = futureTime.getHours() < 12 ? 'AM' : 'PM';
        const label = `${hours}:${minutes} ${period}`;
        timeSlots.push({ label, value: futureTime });
    }

    return timeSlots;
};

const parseTimeString = (timeString) => {
    const [time, modifier] = timeString.split(' '); 
    let [hours, minutes] = time.split(':');

    if (modifier === 'PM' && hours !== '12') {
        hours = parseInt(hours, 10) + 12;
    }
    if (modifier === 'AM' && hours === '12') {
        hours = '00';
    }

    const today = new Date();
    const year = today.getFullYear();
    const month = String(today.getMonth() + 1).padStart(2, '0'); 
    const day = String(today.getDate()).padStart(2, '0');

    const dateTimeString = `${year}-${month}-${day}T${hours}:${minutes}`;

    return new Date(dateTimeString);
};

const Weather = () => {
    const [data, setData] = useState({});
    const [isWatering, setIsWatering] = useState(false);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);
    const [selectedZone, setSelectedZone] = useState('All');
    const [selectedTime, setSelectedTime] = useState(generateTimeSlots()[0].label);
    const [selectedDuration, setSelectedDuration] = useState(durations[0]);
    const [rainData, setRainData] = useState([]);
    const [currentWeather, setCurrentWeather] = useState(null);
    const [currentPrecipitation, setCurrentPrecipitation] = useState(null);

    const latitude = 43.39141047955725;
    const longitude = -79.76961512140457;

    const fetchCache = useCallback(async () => {
        try {
            const response = await axios.get(`${API_BASE_URL}/queue`);
            setData(response.data);
            setLoading(false);
        } catch (error) {
            console.error('Error fetching cache:', error);
            setError(error);
            setLoading(false);
        }
    }, []);

    const fetchWeatherData = useCallback(async () => {
        try {
            const response = await axios.get(`https://api.open-meteo.com/v1/forecast`, {
                params: {
                    latitude: latitude,
                    longitude: longitude,
                    hourly: 'precipitation_probability,temperature_2m',
                    current_weather: true,
                    timezone: 'auto',
                    past_days: 1,
                    forecast_days: 1,
                }
            });
            const data = response.data;
            const times = data.hourly.time;
            const precipitation = data.hourly.precipitation_probability;
            const today = new Date().toISOString().split('T')[0];
            const filteredRainData = times.map((time, index) => ({
                time,
                precipitation: precipitation[index]
            })).filter(item => item.time.startsWith(today));

            setRainData(filteredRainData);
            setCurrentWeather(data.current_weather);

            const currentHour = new Date().getHours();
            const currentPrecipitation = filteredRainData.find(item => new Date(item.time).getHours() === currentHour);
            setCurrentPrecipitation(currentPrecipitation ? currentPrecipitation.precipitation : null);

            setLoading(false);
        } catch (error) {
            console.error('Error fetching weather data:', error);
            setError(error);
            setLoading(false);
        }
    }, [latitude, longitude]);

    const addToQueue = async (task) => {
        try {
            await axios.post(`${API_BASE_URL}/queue`, task);
            fetchCache(); // Immediately fetch the updated cache after adding
        } catch (error) {
            console.error('Error adding to queue:', error);
        }
    };

    const removeFromQueue = async (id) => {
        try {
            await axios.delete(`${API_BASE_URL}/queue/${id}`);
            fetchCache(); // Immediately fetch the updated cache after removing
        } catch (error) {
            console.error('Error removing from queue:', error);
        }
    };

    useEffect(() => {
        fetchCache();
        fetchWeatherData();

        const cacheIntervalId = setInterval(fetchCache, 1000); // Fetch the cache every 1 second
        const weatherIntervalId = setInterval(fetchWeatherData, 60000); 

        return () => {
            clearInterval(cacheIntervalId);
            clearInterval(weatherIntervalId);
        };
    }, [fetchCache, fetchWeatherData]);

    const handleWaterPlants = async () => {
        setIsWatering(true);

        let selectedTimeDate;

        if (selectedTime === 'Now') {
            selectedTimeDate = new Date();
        } else {
            selectedTimeDate = parseTimeString(selectedTime);
            if (isNaN(selectedTimeDate.getTime())) {
                console.error('Invalid time selected:', selectedTime);
                setIsWatering(false);
                return;
            }
        }

        const newTask = {
            zone: selectedZone,
            duration: selectedDuration,
            scheduledTime: selectedTimeDate.toISOString(),
        };

        await addToQueue(newTask);
        setIsWatering(false);
    };

    const handleZoneSelect = (e) => {
        setSelectedZone(e.target.value);
    };

    const handleTimeSelect = (e) => {
        setSelectedTime(e.target.value === 'Now' ? 'Now' : e.target.value);
    };

    const handleDurationSelect = (e) => {
        setSelectedDuration(parseInt(e.target.value, 10));
    };

    const handleRemoveTask = async (index) => {
        const task = data.queue[index];
        await removeFromQueue(task.id);
    };

    if (loading) {
        return <div className="loading-spinner">Loading...</div>;
    }

    if (error) {
        return <div className="error-message">Error fetching data: {error.message}</div>;
    }

    const zones = ['1', '2', '3', '4', 'All'];
    const times = generateTimeSlots();

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
                        <select id="zone-select" value={selectedZone} onChange={handleZoneSelect}>
                            {zones.map((zone, index) => (
                                <option key={index} value={zone}>
                                    {zone}
                                </option>
                            ))}
                        </select>
                    </div>
                    <div>
                        <label htmlFor="time-select">Select Time:</label>
                        <select id="time-select" value={selectedTime} onChange={handleTimeSelect}>
                            {times.map((timeObj, index) => (
                                <option key={index} value={timeObj.label}>
                                    {timeObj.label}
                                </option>
                            ))}
                        </select>
                    </div>
                    <div>
                        <label htmlFor="duration-select">Select Duration:</label>
                        <select id="duration-select" value={selectedDuration} onChange={handleDurationSelect}>
                            {durations.map((duration, index) => (
                                <option key={index} value={duration}>
                                    {duration} minutes
                                </option>
                            ))}
                        </select>
                    </div>
                    <button onClick={handleWaterPlants} className="water-button">
                        {isWatering ? 'Watering...' : 'Water Plants'}
                    </button>
                </div>
            </div>
            <div>
                <h2>Status</h2>
                <div className="status-container">
                    <div className="connection-status">
                        <div>{data.espStatus?.connected ? 'Connected to ESP' : 'ESP is not reachable'}</div>
                    </div>
                    {data.espStatus?.connected && (
                        <>
                            <div className="current-zone-status">
                                <h3>Current Zone</h3>
                                <div>{data.espStatus?.currentZone ? `Zone: ${data.espStatus.currentZone}` : 'No zone is currently being watered'}</div>
                                <div>{data.espStatus?.currentTimer ? `Time: ${data.espStatus.currentTimer} minutes remaining` : ''}</div>
                            </div>
                            <div className="queue-status">
                                <h3>ESP Queue</h3>
                                <div>{data.espStatus?.queue.length > 0 ? data.espStatus.queue.join(', ') : 'No commands in ESP queue'}</div>
                            </div>
                        </>
                    )}
                    <div className="queue-status">
                        <h3>Scheduled Watering Queue</h3>
                        <div className="local-queue">
                            {data.queue.length > 0 ? (
                                data.queue.map((item, index) => (
                                    <div key={index} className="queue-item">
                                        <span>{`Time: ${new Date(item.scheduledTime).toLocaleTimeString()}`}</span>
                                        <span>{` | Zone: ${item.zone}`}</span>
                                        <span>{` | Duration: ${item.duration} minutes  |  `}</span>
                                        <button className="remove-button" onClick={() => handleRemoveTask(index)}>
                                            <span role="img" aria-label="trash can">🗑️</span>
                                        </button>
                                    </div>
                                ))
                            ) : (
                                <div>No commands in queue</div>
                            )}
                        </div>
                    </div>
                </div>
            </div>
        </div>
    );
};

export default Weather;
