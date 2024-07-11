import React, { useState, useEffect } from 'react';
import ImageGallery from "react-image-gallery";

import '../App.css';
import "react-image-gallery/styles/css/image-gallery.css";

import NavBar from '../components/navBar';
import Footer from '../components/footer';

const Gallery = () => {
    const [images, setImages] = useState([]);

    useEffect(() => {

        const imageList = [
            {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            },
            {
                original: "https://i.pinimg.com/736x/13/04/7c/13047cc05340be6afe126ca5906591fe.jpg",
                thumbnail: "https://i.pinimg.com/736x/13/04/7c/13047cc05340be6afe126ca5906591fe.jpg",
            },
            {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            },
            {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            }, {
                original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
                thumbnail: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
            },
        ];

        const shuffledImages = imageList.sort(() => Math.random() - 0.5);
        // const shuffledImages = imageList;

        setImages(shuffledImages);
    }, []);

    return (
        <div className="App">
            <NavBar />
            <main className="App-main">
                <section>
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
                </section>
            </main>

            <Footer />
        </div>
    );
}

export default Gallery;