import ImageGallery from "react-image-gallery";

import '../App.css';

import NavBar from '../components/navBar';
import Footer from '../components/footer';

const images = [
    {
        original: "https://img.freepik.com/free-photo/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
    },
    {
        original: "https://img.freepik.com/free-phto/painting-mountain-lake-with-mountain-background_188544-9126.jpg",
    },
];


const Gallery = () => {
    return (
        <div className="App">
            <NavBar />
            <main className="App-main">
                <section>
                    <ImageGallery items={images} />
                </section>
            </main>
            <Footer />
        </div>
    );
}

export default Gallery;