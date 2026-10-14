import { findProteinsInSequence } from './api.js';

const proteinsSlot = document.getElementById('find-proteins-container');
const buttonsContainer = document.createElement('div');
buttonsContainer.classList.add('button-group');

// The proteins in seq-proteins.txt belong to the Wuhan genome
const sequence = "Wuhan";

const button = document.createElement('button');
button.id = "btn-proteins";
button.textContent = sequence;
buttonsContainer.appendChild(button);
proteinsSlot.appendChild(buttonsContainer);


const resultSlot = document.createElement('div');
const proteinsText = document.createElement('div');
proteinsSlot.appendChild(resultSlot);
proteinsSlot.appendChild(proteinsText);

/*
    One line per protein, with the columns aligned:
        QHD43415_1    180 aa   +2   265 – 804       MESL   ATG GAG AGC CTT
    A protein with a frameshift gets a second line explaining both segments:
                      frameshift -1 · aminoacids 1–9 in frame +2, 10–932 in frame +1 from base 13467 (read twice)
*/
function proteinsToText(response) {
    const nameWidth = Math.max(...response.proteins.map(p => p.name.length)) + 2;
    const indent = ' '.repeat(nameWidth);
    const lines = [];

    for (const p of response.proteins) {
        const name = p.name.padEnd(nameWidth);
        const length = `${p.length} aa`.padEnd(9);

        if (!p.found) {
            lines.push(`${name}${length}not found`);
            continue;
        }

        const frame = `${p.strand}${p.frame}`.padEnd(5);
        const indexes = `${p.begin} – ${p.end}`.padEnd(16);
        lines.push(`${name}${length}${frame}${indexes}${p.firstAminoacids.padEnd(7)}${p.firstCodons.join(' ')}`);

        if (p.frameshift) {
            const slip = p.slip < 0 ? `${p.slip}` : `+${p.slip}`;
            const base = p.slip < 0 ? 'read twice' : 'skipped';
            lines.push(`${indent}frameshift ${slip} · aminoacids 1–${p.prefixLength} in frame ${p.strand}${p.frame}, `
                + `${p.prefixLength + 1}–${p.length} in frame ${p.strand}${p.shiftFrame} from base ${p.shiftBegin} (${base})`);
        }
    }
    return lines.join('\n');
}

button.addEventListener('click', async () => {
    button.classList.add('is-active', 'is-loading');
    button.disabled = true;

    try {
        const response = await findProteinsInSequence(sequence);

        resultSlot.innerHTML = `
            <div class="stats">
                <div class="stat"><span class="stat-label">Found</span><span class="stat-value">${response.found} / ${response.total}</span></div>
                <div class="stat"><span class="stat-label">With frameshift</span><span class="stat-value">${response.frameshifts}</span></div>
                <div class="stat"><span class="stat-label">Genome bases</span><span class="stat-value">${response.genomeLength}</span></div>
            </div>`;

        proteinsText.innerHTML = '';
        const label = document.createElement('span');
        label.classList.add('result-label');
        label.textContent = 'Protein · length · frame · genome indexes (0-based) · first 4 aminoacids · codons';

        const text = document.createElement('p');
        text.classList.add('sequence-text', 'comparison-text');
        text.textContent = proteinsToText(response);

        proteinsText.append(label, text);
    } catch (error) {
        resultSlot.innerHTML = '';
        proteinsText.innerHTML = '';
        const message = document.createElement('p');
        message.classList.add('result-title', 'is-empty');
        message.textContent = `Could not search the proteins: ${error.message}`;
        proteinsText.appendChild(message);
    } finally {
        button.classList.remove('is-loading');
        button.disabled = false;
    }
});
