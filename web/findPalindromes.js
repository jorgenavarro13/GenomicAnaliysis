import { findPalindromeInSequence } from './api.js';

const findSequenceSlot = document.getElementById('find-palindromes-container');
const buttonsContainer = document.createElement('div');
buttonsContainer.classList.add('button-group');

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
    button.id = "btn-palindrome" + buttonName
    button.textContent = innerText;

    buttonsContainer.appendChild(button);
}
findSequenceSlot.appendChild(buttonsContainer);


const resultSlot   = document.createElement('div')
const palindromeSlot = document.createElement('div')
for (const buttonKey in buttons){
    const buttonName = "btn-palindrome" + buttonKey
    const genToFind = buttons[buttonKey];

    const button = document.getElementById(buttonName)

    button.addEventListener('click', async () => {
        buttonsContainer.querySelectorAll('button').forEach(b => b.classList.remove('is-active'));
        button.classList.add('is-active', 'is-loading');

        var response = await findPalindromeInSequence(genToFind);
        button.classList.remove('is-loading');

        resultSlot.innerHTML = `
            <div class="stats">
                <div class="stat"><span class="stat-label">Begin</span><span class="stat-value">${response.begin}</span></div>
                <div class="stat"><span class="stat-label">End</span><span class="stat-value">${response.end}</span></div>
                <div class="stat"><span class="stat-label">Length</span><span class="stat-value">${response.end - response.begin + 1}</span></div>
            </div>`;
        palindromeSlot.innerHTML = `
            <span class="result-label">Palindrome</span>
            <p class="sequence-text">${response.palindrome.join('')}</p>`;
    })
}

findSequenceSlot.appendChild(resultSlot)
findSequenceSlot.appendChild(palindromeSlot)
