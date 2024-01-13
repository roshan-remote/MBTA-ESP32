#ifndef CSS_H
#define CSS_H

char webCSS[] PROGMEM = R"=====(
* {
font-family: sans-serif;
}

body {
padding: 1em;
font-size: 1em;
text-align: left;
}

table {
border-collapse: collapse;
width: 50%;
border: 2px solid #ccc;
}

th,
td {
border: 1px solid #ccc;
padding: 8px;
text-align: center;
}

th {
background-color: #f2f2f2;
}

tr:nth-child(even) {
background-color: #f9f9f9;
}

tr:hover {
background-color: #ddd;
}

#red {
color: red;
}

#green {
color: green;
}

#blue {
color: blue;
}

.header {
display: flex;
align-items: center;
justify-content: space-between;
padding: 20px;
}

.t-image {
position: relative;
top: -12px;
}

.mirrored {
transform: scaleX(-1);
}

.custom-button {
padding: 10px 10px;
font-size: 12px;
background-color: #3498db;
color: white;
border: none;
border-radius: 6px;
cursor: pointer;
margin: 2px;
}

.custom-red {
background-color: red;
}

.custom-green {
background-color: green;
}

.transposed-table {
writing-mode: vertical-lr;
}

.transposed-table th, .transposed-table td {
white-space: nowrap;
writing-mode: horizontal-tb;
}

*, *::after, *::before {
box-sizing: border-box;
}

@keyframes rotate {
0% {
transform: rotate(0);
}

100% {
transform: rotate(360deg);
}
}

@keyframes text-color {
0% {
color: rgba(0, 0, 0, 1);
}

50% {
color: rgba(0, 0, 0, .05);
}

100% {
color: rgba(0, 0, 0, .01);
}
}

.spinner-2 {
width: 300px;
height: 300px;
display: flex;
justify-content: center;
align-items: center;
font-size: 2rem;
overflow: hidden;
position: relative;
animation: text-color 2s ease-in-out infinite alternate;
}

.spinner-2::before,
.spinner-2::after {
content: "";
position: absolute;
width: 100%;
height: 100%;
border-radius: 50%;
border: 15px solid transparent;
mix-blend-mode: overlay;
animation: rotate var(--duration) var(--timing) infinite;
pointer-events: none;
}

.spinner-2::before {
border-left-color: lightblue;
--duration: 2s;
--timing: ease-in;
}

.spinner-2::after {
border-right-color: lightgreen;
--duration: 2.5s;
--timing: ease-out;
}

.loggerBox {
min-height: 300px;
max-height: 400px;
overflow-y: scroll;
background-color: black;
color: white;
padding: 20px;
white-space: pre-wrap; /* Preserve line breaks */
}
)=====";

#endif