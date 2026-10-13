import { requestSequence } from './api.js';
import { createVisualSequence } from './genContainer.js';

const fileSlot = document.getElementById('file');
const buttonsContainer = document.createElement('div');
buttonsContainer.classList.add('button-group');

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

    buttonsContainer.appendChild(button);
}
fileSlot.appendChild(buttonsContainer);



var file = document.createElement('h3');
var sequence = document.createElement('div');
file.classList.add('result-title', 'is-empty');
file.textContent = 'No sequence yet';
fileSlot.appendChild(file);
fileSlot.appendChild(sequence);

for (const buttonKey in buttons){
    const buttonName = "btn-" +buttonKey
    const fileToRequest = buttons[buttonKey];

    const button = document.getElementById(buttonName)

    button.addEventListener('click', async () => {
        buttonsContainer.querySelectorAll('button').forEach(b => b.classList.remove('is-active'));
        button.classList.add('is-active', 'is-loading');

        var response = await requestSequence(fileToRequest);
        button.classList.remove('is-loading');

        // "../data/gen-M.txt" -> "gen-M"
        file.textContent = response.file.split('/').pop().replace('.txt', '');
        file.classList.remove('is-empty');

        sequence.innerHTML = '';
        createVisualSequence(sequence, response.sequence)

        const label = document.createElement('span');
        label.classList.add('result-label');
        label.textContent = `Secuencia · ${response.sequence.length} bases`;
        label.style.marginTop = '32px';

        const text = document.createElement('p');
        text.classList.add('sequence-text');
        text.textContent = response.sequence.join('');

        sequence.append(label, text);
    })
}
