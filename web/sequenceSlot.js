import { requestSequence } from './api.js';
import { createVisualSequence } from './genContainer.js';

const fileSlot = document.getElementById('file');
const buttonsContainer = document.createElement('div');

const buttons = {
    WuhanButton : "Wuhan",
    TexasButton : "Texas",
    MButton : "M",
    SButton : "S",
    ORF1ABButton : "ORF1AB"
}


for(const buttonKey in buttons ){
    const buttonName = buttonKey
    const innerText = buttons[buttonKey];

    const  button = document.createElement('button');
    button.name = buttonName;
    button.id = "btn-" + buttonName
    button.textContent = innerText;
    console.log(button.id)

    buttonsContainer.appendChild(button);
}
fileSlot.appendChild(buttonsContainer);



var file = document.createElement('h1');
var sequence = document.createElement('p');
file.textContent = 'No sequence yet';
fileSlot.appendChild(file);
fileSlot.appendChild(sequence);

for (const buttonKey in buttons){
    const buttonName = "btn-" +buttonKey
    const fileToRequest = buttons[buttonKey];

    const button = document.getElementById(buttonName)
    console.log(buttonName)

    button.addEventListener('click', async () => {
        var response = await requestSequence(fileToRequest);

        file.textContent = response.file;
        sequence.innerHTML = response.sequence;
        
        createVisualSequence(sequence, response.sequence)
    })
}
