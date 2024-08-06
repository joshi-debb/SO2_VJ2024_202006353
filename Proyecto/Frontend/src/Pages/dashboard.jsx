import React, { useState } from 'react';
import { Chart as ChartJS, ArcElement, Tooltip, Legend } from "chart.js";
import { Doughnut } from 'react-chartjs-2';
ChartJS.register(ArcElement, Tooltip, Legend);
import ChartDataLabels from 'chartjs-plugin-datalabels'; // Importa el plugin
import '../Diseño/tabla.css';

export const Dash = ({ ListParms }) => {
  if (!ListParms || !ListParms.ListDatos) {
    return null;
  };

  const generateRandomColor = () => {
    const r = Math.floor(Math.random() * 256);
    const g = Math.floor(Math.random() * 256);
    const b = Math.floor(Math.random() * 256);
    return `rgba(${r}, ${g}, ${b}, 0.6)`;
  };

  // Ordenar los procesos por memoria en orden descendente
  const sortedData = ListParms.ListTable
    .map((dato, index) => ({
      ...dato,
      memoria: ListParms.ListDatos[index]
    }))
    .sort((a, b) => b.memoria - a.memoria);

  // Obtener los primeros 10 procesos y agrupar el resto en "otros"
  const top10Data = sortedData.slice(0, 10);
  const otherData = sortedData.slice(10);

  const otherMemory = otherData.reduce((acc, curr) => acc + curr.memoria, 0);

  const data = {
    labels: [
      ...top10Data.map(dato => dato.name + " - " + dato.pid),
      ...(otherData.length > 0 ? ["Otros"] : [])
    ],
    datasets: [
      {
        data: [
          ...top10Data.map(dato => dato.memoria),
          ...(otherData.length > 0 ? [otherMemory] : [])
        ],
        backgroundColor: [
          ...top10Data.map(() => generateRandomColor()),
          ...(otherData.length > 0 ? [generateRandomColor()] : [])
        ]
      },
    ],
  };

  const opciones = {
    responsive: true,
    maintainAspectRatio: false,
    plugins: {
      legend: {
        position: 'bottom',
      },
      title: {
        display: true,
        text: 'Chart.js Doughnut Chart'
      },
      datalabels: { // Configuración del plugin datalabels
        formatter: (value, ctx) => { // Formateador para personalizar los datos en cada segmento
          let label = ctx.chart.data.labels[ctx.dataIndex]; // Obtiene el label del segmento
          return label;
        },
        color: 'black', // Color del texto
        display: 'auto' // Muestra el valor en todos los segmentos
      }
    }
  };

  return (
    <div style={{ display: 'flex' }}>
      <div style={{ width: '500px', height: '500px' }}>
        <Doughnut data={data} options={opciones} plugins={[ChartDataLabels]} />
      </div>
      <div className="scroleados">
        <div className="tabla-container">
          <table className="tabla">
            <thead>
              <tr>
                <th>PID</th>
                <th>Nombre</th>
                <th colSpan="2">Memoria</th>
              </tr>
            </thead>
            <tbody>
              {ListParms.ListTable.map((dato) => (
                <tr key={dato.pid}>
                  <td>{dato.pid}</td>
                  <td>{dato.name}</td>
                  <td>{dato.memoria}KB</td>
                  <td>{dato.cpu}%</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>

      </div>

    </div>
  );
};
