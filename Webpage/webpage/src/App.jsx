import React from 'react';
import { BrowserRouter, Routes, Route } from "react-router-dom";

import './App.css';

import Gallery from './pages/gallery';
import Home from './pages/home';
import About from './pages/about';

function App() {
  return (
    <BrowserRouter>
      <Routes>
          <Route index path="/" element={<Home />} />
          <Route path="gallery" element={<Gallery />} />
          <Route path="about" element={<About />} />
      </Routes>
    </BrowserRouter>
  );
};

export default App;
