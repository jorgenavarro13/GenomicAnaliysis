import { requestSequence } from './api.js';
const fileSlot = document.getElementById('file');

var file = document.createElement('h1');
file.textContent = 'No file loaded';
fileSlot.appendChild(file);



const WuhanButton = document.createElement('button');
WuhanButton.textContent = "Wuhan";

fileSlot.appendChild(WuhanButton);

WuhanButton.addEventListener('click', async () => {
    var response = await requestSequence("Wuhan");
    console.log(response)

    file.textContent = response.file;
    fileSlot.innerHTML = response.sequence;

});


const TexasButton = document.createElement('button');
TexasButton.textContent = "Texas";

fileSlot.appendChild(TexasButton);

TexasButton.addEventListener('click', async () => {
    var response = await requestSequence("Texas");
    console.log(response)

    file.textContent = response.file;
    fileSlot.innerHTML = response.sequence;
});





