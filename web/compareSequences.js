import { compareSequences } from './api.js';

const compareSlot = document.getElementById('compare-sequences-container');
const buttonsContainer = document.createElement('div');
buttonsContainer.classList.add('button-group');

// Only Wuhan vs Texas: they are aligned (same start), so the server can compare them base by base
const sequenceA = "Wuhan";
const sequenceB = "Texas";

const button = document.createElement('button');
button.id = "btn-compare";
button.textContent = `${sequenceA} vs ${sequenceB}`;
buttonsContainer.appendChild(button);
compareSlot.appendChild(buttonsContainer);


const resultSlot = document.createElement('div');
const differencesSlot = document.createElement('div');
compareSlot.appendChild(resultSlot);
compareSlot.appendChild(differencesSlot);

/*
    The server returns the codons grouped by frame (frames[0], frames[1], frames[2]).
    To read it per mutation we group them by the base that differs:
        Base 8781
          Frame 0 · CCA → TCA · Pro → Ser · changes the aminoacid
          Frame 1 · ...
    A frame can have no entry for a base when that codon was already reported by a previous
    difference, or when the base is before the first codon of the frame (index 0 or 1).
*/
function differencesToText(response) {
    const lines = [];
    for (const index of response.differences) {
        lines.push(`Base ${index}`);

        response.frames.forEach((frame, frameNumber) => {
            const d = frame.find(difference => difference.index === index);
            if (!d) {
                lines.push(`  Frame ${frameNumber} · no new codon (already reported or outside the frame)`);
                return;
            }
            const effect = d.changesAminoacid ? 'changes the aminoacid' : 'silent mutation';
            lines.push(`  Frame ${frameNumber} · codon ${d.codonStart} · ${d.codonA} → ${d.codonB} · ${d.aminoA} → ${d.aminoB} · ${effect}`);
        });
        lines.push('');
    }
    return lines.join('\n');
}

button.addEventListener('click', async () => {
    button.classList.add('is-active', 'is-loading');
    button.disabled = true;

    try {
        const response = await compareSequences(sequenceA, sequenceB);

        resultSlot.innerHTML = `
            <div class="stats">
                <div class="stat"><span class="stat-label">Differences</span><span class="stat-value">${response.differences.length}</span></div>
                <div class="stat"><span class="stat-label">Compared bases</span><span class="stat-value">${response.comparedLength}</span></div>
                <div class="stat"><span class="stat-label">Longest without differences</span><span class="stat-value">${response.longest.length}</span></div>
                <div class="stat"><span class="stat-label">From – To</span><span class="stat-value">${response.longest.begin} – ${response.longest.end}</span></div>
            </div>`;

        differencesSlot.innerHTML = '';
        const label = document.createElement('span');
        label.classList.add('result-label');
        label.textContent = 'Differences per reading frame · indexes are 0-based';

        const text = document.createElement('p');
        text.classList.add('sequence-text', 'comparison-text');
        text.textContent = differencesToText(response);

        differencesSlot.append(label, text);
    } catch (error) {
        resultSlot.innerHTML = '';
        differencesSlot.innerHTML = '';
        const message = document.createElement('p');
        message.classList.add('result-title', 'is-empty');
        message.textContent = `Could not compare the sequences: ${error.message}`;
        differencesSlot.appendChild(message);
    } finally {
        button.classList.remove('is-loading');
        button.disabled = false;
    }
});
