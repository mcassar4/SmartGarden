import React, { useState, useEffect } from 'react';
import axios from 'axios';
import '../styles/style.css';

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

const getCurrentHourPlusOne = () => {
    const date = new Date();
    let nextHour = date.getHours() + 1;
    if (nextHour >= 24) nextHour = 0;
    const hour = nextHour % 12 === 0 ? 12 : nextHour % 12;
    const period = nextHour < 12 ? 'AM' : 'PM';
    return `${hour}:00 ${period}`;
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

        checkPrecipitationAtMidnight();

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

    const executeWatering = async () => {
        const sendWaterRequest = async (zone) => {
            const min = zone === 2 ? 80 : 70;
            const data = `${zone}:${min}`;
            console.log("Data sending: " + data);
            try {
                await axios.post('http://172.20.10.11/esp_rx', data).then((response) => {
                    if (!response.data || response.data !== 'rec') {
                        console.log('No response from ESP');
                    }
                    console.log('Data sent successfully:', response.data);
                });
            } catch (error) {
                console.error('Error watering plants:', error);
            }
        };

        if (selectedZone === 'All') {
            for (let i = 1; i <= 5; i++) {
                await sendWaterRequest(i);
            }
        } else {
            await sendWaterRequest(selectedZone);
        }
    };

    const handleWaterPlants = async () => {
        setIsWatering(true);
        if (selectedHour === 'Now') {
            console.log('Watering now');
            setIsWatering(false);
            await executeWatering();
        } else {
            setIsWatering(false);
            console.log('Watering will start at', selectedHour);
            const selectedHourDate = new Date();
            const [hour, period] = selectedHour.split(' ');
            selectedHourDate.setHours(
                period === 'AM' ? parseInt(hour, 10) % 12 : parseInt(hour, 10) % 12 + 12,
                0,
                0,
                0
            );

            const delay = selectedHourDate.getTime() - new Date().getTime();
            if (delay > 0) {
                await new Promise(resolve => setTimeout(resolve, delay));
            }
            await executeWatering();
        }
    };

    // This is for get request
    // try {
    //     axios.get('http://172.20.10.11/esp_tx').then((response) => {
    //         console.log(JSON.stringify(response.data['key1']));
    //         setIsWatering(false);
    //     });
    // } catch (error) {
    //     console.error('Error watering plants:', error);
    //     setIsWatering(false);
    // }

    const zones = ['1', '2', '3', '4', '5', 'All'];
    const hours = generateHours();
    const defaultZone = 'All';

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
                            {hours.map((hourObj, index) => (
                                <option key={index} value={hourObj.label} disabled={hourObj.disabled}>
                                    {hourObj.label}
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
