import { findPalindromeInSequence } from './api.js';
import { createVisualSequence } from './genContainer.js';

const findSequenceSlot = document.getElementById('find-palindromes-container');
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
    button.id = "btn-palindrome" + buttonName
    button.textContent = innerText;
    console.log(button.id)
    
    buttonsContainer.appendChild(button);
}
findSequenceSlot.appendChild(buttonsContainer);


const resultSlot   = document.createElement('h1')
const palindromeSlot = document.createElement('p')
for (const buttonKey in buttons){
    const buttonName = "btn-palindrome" + buttonKey
    const genToFind = buttons[buttonKey];

    const button = document.getElementById(buttonName)
    console.log(buttonName)

    button.addEventListener('click', async () => {
        var response = await findPalindromeInSequence(genToFind);
        resultSlot.innerHTML = `Begin: ${response.begin}, End: ${response.end}, Length: ${response.end - response.begin + 1}`;
        palindromeSlot.innerHTML = `Palindrome: ${response.palindrome}`;
    })
}

findSequenceSlot.appendChild(resultSlot)
findSequenceSlot.appendChild(palindromeSlot)

