const chain = ['A', 'A', 'A', 'T', 'A', 'G', 'C', 'T', 'A', 'G', 'C', 'T', 'A', 'A', 'A', 'T', 'A', 'G', 'C', 'T', 'A', 'G', 'C', 'T', 'A', 'A', 'A', 'T', 'A', 'G', 'C', 'T', 'A', 'G', 'C', 'T', 'A', 'A', 'A', 'T', 'A', 'G', 'C', 'T', 'A', 'G', 'C', 'T', 'A', 'A', 'A', 'T', 'A', 'G', 'C', 'T', 'A', 'G', 'C', 'T', 'A', 'A', 'A', 'T', 'A', 'G', 'C', 'T', 'A', 'G', 'C', 'T', 'A', 'A', 'A', 'T', 'A', 'G', 'C', 'T', 'A', 'G', 'C', 'T', 'A', 'A', 'A', 'T', 'A', 'G', 'C', 'T', 'A', 'G', 'C', 'T'];
const colorMapping = {
    'A' : "#6a3d7c",
    'G' : "#ee7a62",
    'C' : "#f4d36a",
    'T' : "#a9c4a1"
}

const oppositeMapping = {
    'A' : 'T',
    'G' : 'C',
    'T' : 'A',
    'C' : 'G'
}

function createVisualSequence(element, sequence){
    const basesPerTurn = 20.5;  
    const spacing = 28;         // Space between pairs (px)
    const amplitude = 30;       
    const helixWidth = sequence.length * spacing;
    const middle = amplitude + 10;
    const minHalfHeight = 12;   // Keep the letters visible where the strands cross 
    const visibleBases = 20;    // Pares visibles en sliding window
    
    // Angle of the helix at a horizontal position
    function angleAt(x) {
        return (2 * Math.PI * x) / (basesPerTurn * spacing);
    }
    
    const helix = document.createElement('div');
    helix.classList.add('helix');
    helix.style.width = `${helixWidth}px`;
    helix.style.height = `${middle * 2}px`;
    
    // Base pairs: one bar per pair, split into two colored halves
    for (let i = 0; i < sequence.length; i++) {
        const x = i * spacing + spacing / 2;
        const offset = amplitude * Math.sin(angleAt(x));
        const halfHeight = Math.max(Math.abs(offset), minHalfHeight);
        const base = sequence[i];
        const opposite = oppositeMapping[base];
        
        const pair = document.createElement('div');
        pair.classList.add('pair');
        pair.style.left = `${x}px`;
        pair.style.top = `${middle - halfHeight}px`;
        pair.style.height = `${halfHeight * 2}px`;
        
        // The first strand sits below the middle when offset > 0
        const [topBase, bottomBase] = offset > 0 ? [opposite, base] : [base, opposite];
        for (const letter of [topBase, bottomBase]) {
            const nucleotide = document.createElement('div');
            nucleotide.classList.add('nucleotid');
            nucleotide.style.backgroundColor = colorMapping[letter];
            nucleotide.textContent = letter;
            pair.append(nucleotide);
        }
        
        helix.append(pair);
    }
    
    // Sliding window
    const helixWindow = document.createElement('div');
    helixWindow.classList.add('helix-window');
    helixWindow.tabIndex = 0;   // lets the arrow keys scroll it once clicked
    helixWindow.style.width = `${Math.min(visibleBases, sequence.length) * spacing}px`;
    helixWindow.append(helix);

    const positionLabel = document.createElement('p');
    positionLabel.classList.add('helix-position');
    
    element.append(helixWindow, positionLabel);
    
    function updatePosition() {
        const windowStart = Math.round(helixWindow.scrollLeft / spacing);
        const windowEnd = Math.min(windowStart + visibleBases, sequence.length);
        positionLabel.textContent = `${windowStart + 1}–${windowEnd} of ${sequence.length}`;
    }
    
    helixWindow.addEventListener('scroll', updatePosition);
    updatePosition();
}


const genContainer = document.getElementById('gen-container');
createVisualSequence(genContainer, chain);

export {createVisualSequence}