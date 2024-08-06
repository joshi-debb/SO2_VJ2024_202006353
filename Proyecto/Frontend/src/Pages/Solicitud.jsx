import React, { useState } from 'react';

import "../Diseño/tabla.css"
export const Solicitud = ({ TableList }) => {
    return (
        <div className="scroleados">
            <div className="tabla-container">
                <table className="tabla">
                    <thead>
                        <tr>
                            <th>PID</th>
                            <th>Llamada</th>
                            <th>Tamaño</th>
                            <th>Fecha</th>
                        </tr>
                    </thead>
                    <tbody>
                        {TableList.map((dato) => (
                            <tr key={dato.pid}>
                                <td>{dato.pid}</td>
                                <td>{dato.call}</td>
                                <td>{dato.size}KB</td>
                                <td>{dato.fecha}</td>
                            </tr>
                        ))}
                    </tbody>
                </table>
            </div>
        </div>

    );
};