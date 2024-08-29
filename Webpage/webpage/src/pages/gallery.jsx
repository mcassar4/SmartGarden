import React, { useState, useEffect } from 'react';
import ImageGallery from "react-image-gallery";

import '../styles/style.css';
import "react-image-gallery/styles/css/image-gallery.css";

import NavBar from '../components/navBar';
import Footer from '../components/footer';

const importAll = (r) => {
    return r.keys().map(r);
};

const Gallery = () => {
    const [images, setImages] = useState([]);

    useEffect(() => {
        const imageList = importAll(require.context('../../public/images/', false, /\.(png|jpg|svg|jfif)$/));
        const formattedImages = imageList.map(image => ({
            original: image,
            thumbnail: image,
        }));
        const shuffledImages = formattedImages.sort(() => Math.random() - 0.5);

        setImages(shuffledImages);
    }, []);

    return (
        <div className="App">
            <NavBar />
            <main className="App-main">
                <div className="gallery-container">
                    <ImageGallery items={images} showThumbnails={true}
                        showFullscreenButton={true}
                        showPlayButton={true}
                        autoPlay={true}
                        slideInterval={5000}
                        additionalClass="image-gallery"
                        infinite={true}
                        showNav={true}
                        thumbnailPosition='top'
                        showIndex={true}
                        showBullets={true}
                    />
                </div>
            </main>
            <Footer />
        </div>
    );
}

export default Gallery;