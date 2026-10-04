
export const C_API_URL = 'http://localhost:8080';

const sequence_options = {
    "Wuhan"  : "../data/SARS-COV-2-MN908947.3.txt",
    "M": "../data/gen-M.txt",
    "ORF1AB" : "../data/gen-ORF1AB.txt",
    "S" : "../data/gen-S.txt",
    "Texas": "../data/SARS-COV-2-MT106054.1.txt",
};

export async function requestSequence(sequence) {
    if (!sequence || sequence_options[sequence] === undefined) throw new Error('No sequence provided');

    const res = await fetch(`${C_API_URL}/sequence`, {
    method: 'POST',
    body: JSON.stringify({ file : sequence_options[sequence] }),
  });

  if (!res.ok) throw new Error(`POST /sequence → ${res.status}`);
  return res.json();
}