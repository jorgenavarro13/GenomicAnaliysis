
const C_API_URL = 'http://localhost:8080';

const sequence_options = {
    "Wuhan"  : "../data/SARS-COV-2-MN908947.3.txt",
    "M": "../data/gen-M.txt",
    "ORF1AB" : "../data/gen-ORF1AB.txt",
    "S" : "../data/gen-S.txt",
    "Texas": "../data/SARS-COV-2-MT106054.1.txt",
};

async function requestSequence(sequence) {
    if (!sequence || sequence_options[sequence] === undefined) throw new Error('No sequence provided');

    const res = await fetch(`${C_API_URL}/sequence`, {
    method: 'POST',
    body: JSON.stringify({ file : sequence_options[sequence] }),
  });

  if (!res.ok) throw new Error(`POST /sequence → ${res.status}`);
  return res.json();
}

async function findGenInSequence(genToFind) {
    if (!genToFind || sequence_options[genToFind] === undefined) throw new Error('No gene to find provided');

    const res = await fetch(`${C_API_URL}/findGenInSequence`, {
    method: 'POST',
    body: JSON.stringify({ 
      gene_file : sequence_options[genToFind],
      sequence_file : sequence_options["Wuhan"] 
    }),
  });

  if (!res.ok) throw new Error(`POST /findGenInSequence → ${res.status}`);
  return res.json();
}

async function findPalindromeInSequence(sequence) {
    if (!sequence || sequence_options[sequence] === undefined) throw new Error('No sequence provided');

    const res = await fetch(`${C_API_URL}/findPalindrome`, {
    method: 'POST',
    body: JSON.stringify({ 
      sequence_file : sequence_options[sequence],
      sequence_name : sequence
    }),
  });

  if (!res.ok) throw new Error(`POST /findPalindrome → ${res.status}`);
  return res.json();
}


export { requestSequence, findGenInSequence, findPalindromeInSequence };