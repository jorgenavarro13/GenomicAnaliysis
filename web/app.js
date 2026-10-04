const chain = ['A', 'G', 'C', 'T', 'A', 'G', 'C', 'T', 'A', 'G', 'C', 'T'];
const hello = document.getElementsByClassName('hello')[0];

const colorMapping = {
    'A' : "orange",
    'G' : "green",
    'C' : "blue",
    'T' : "gray"
}

const oppositeMapping = {
    'A' : 'T',
    'G' : 'C',
    'T' : 'A',
    'C' : 'G'
}

const genContainer = document.getElementById('gen-container');


const horizontalContainer = document.createElement('div');
horizontalContainer.classList.add('horizontal-container');



for(let i=0; i<chain.length; i++){
    let pair = document.createElement('div');
    pair.id = i;
    let nucleuotid = document.createElement('div');
    nucleuotid.classList.add("nucleotid");
    nucleuotid.style.backgroundColor = colorMapping[chain[i]];
    
    pair.classList.add("pair");
    pair.append(nucleuotid);

    let oppositeNucleotide = document.createElement('div');
    oppositeNucleotide.classList.add("nucleotid");
    oppositeNucleotide.style.backgroundColor = colorMapping[oppositeMapping[chain[i]]];

    pair.append(oppositeNucleotide);

    horizontalContainer.append(pair);
    let line = document.createElement('div');
    line.classList.add("line");
    horizontalContainer.append(line);
}

let superiorHelixLine = document.createElement('div');
superiorHelixLine.classList.add('helix-line');

let inferiorHelixLine = document.createElement('div');
inferiorHelixLine.classList.add('helix-line')

genContainer.append(superiorHelixLine);
genContainer.append(horizontalContainer);
genContainer.append(inferiorHelixLine);

console.log(genContainer);