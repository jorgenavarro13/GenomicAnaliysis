import { findGenInSequence } from './api.js';
import { createVisualSequence } from './genContainer.js';

const findSequenceSlot = document.getElementById('find-sequences-container');
const buttonsContainer = document.createElement('div');

const buttons = {
    MButton : "M",
    SButton : "S",
    ORF1ABButton : "ORF1AB"
}

for(const buttonKey in buttons ){
    const buttonName = buttonKey
    const innerText = buttons[buttonKey];
    
    const  button = document.createElement('button');
    button.name = buttonName;
    button.id = "btn-find" + buttonName
    button.textContent = innerText;
    console.log(button.id)
    
    buttonsContainer.appendChild(button);
}
findSequenceSlot.appendChild(buttonsContainer);


const resultSlot   = document.createElement('h1')
const sequenceFound = document.createElement('p')

for (const buttonKey in buttons){
    const buttonName = "btn-find" + buttonKey
    const genToFind = buttons[buttonKey];

    const button = document.getElementById(buttonName)
    console.log(buttonName)

    button.addEventListener('click', async () => {
        var response = await findGenInSequence(genToFind);
        resultSlot.innerHTML = `Begin: ${response.begin}, End: ${response.end}`;
        sequenceFound.innerHTML = `Found: ${response.found}`;
    })
}

findSequenceSlot.appendChild(resultSlot)
findSequenceSlot.appendChild(sequenceFound)
