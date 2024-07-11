import React from "react";
import NavBar from "../components/navBar";
import Footer from "../components/footer";

const About = () => {
    return (
        <div className="App">
            <NavBar />
            <main className="App-main">
                <section>
                    <p>The smart garden project at Evertz. A zone-divided raised garden bed is operated using motorized valves controlled by a microntroller and interfaced with an internally hosted website
                    </p>
                </section>
            </main>
            <Footer />
        </div>
    );
};

export default About;
