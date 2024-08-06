import React, { useState, useEffect } from 'react';
import { Dash } from "./Pages/dashboard";
import { Solicitud } from "./Pages/Solicitud";
import "./Diseño/App.css";

function App() {
  const [mostrarDash, setMostrarDash] = useState(false);
  const [mostrarSol, setMostrarSol] = useState(false);
  const [global, setGlobal] = useState({});
  const [listS, setListS] = useState([]);

  // Función para obtener datos del dashboard y actualizar cada 3 segundos
  useEffect(() => {
    const obtenerDatosDashboard = () => {
      fetch("http://localhost:3000/dashboard", {
        method: "GET",
        headers: {
          "Content-Type": "application/json",
        },
      })
      .then((response) => response.json())
      .then((data) => {
        setGlobal(data);
      })
      .catch((error) => {
        console.error("Error:", error);
      });
    };

    obtenerDatosDashboard();
    setMostrarDash(true); // Mostrar dashboard inicialmente
    const intervalId = setInterval(obtenerDatosDashboard, 3000);

    return () => clearInterval(intervalId);
  }, []);

  const mostrarDashboard = () => {
    setMostrarSol(false);
    setMostrarDash(true);
  };

  const mostrarSolicitudes = () => {
    setMostrarDash(false);
    setMostrarSol(true);

    fetch("http://localhost:3000/solicitud", {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
      },
    })
    .then((response) => response.json())
    .then((data) => {
      setListS(data);
    })
    .catch((error) => {
      console.error("Error:", error);
    });
  };

  return (
    <div className="container">
      <h1>SO2 Proyecto Grupo1</h1>
      <div className="button-container">
        <button onClick={mostrarDashboard}>Dashboard</button>
        <button onClick={mostrarSolicitudes}>Solicitudes</button>
      </div>
      <div className="content">
        {mostrarDash && <Dash ListParms={global} />}
        {mostrarSol && <Solicitud TableList={listS} />}
      </div>
    </div>
  );
}

export default App;
