import NavBar from "../components/navBar";
import Footer from "../components/footer";

const Home = () => {
    return (
        <div className="App">
            <NavBar />
            <main className="App-main">
                <section>
                    <p>Weather forecast</p>
                </section>
            </main>
            <Footer />
        </div>
    );
};

export default Home;
