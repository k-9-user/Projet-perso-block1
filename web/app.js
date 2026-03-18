const statusEl = document.getElementById("status");
const counterEl = document.getElementById("counter");
const chainListEl = document.getElementById("chainList");
const template = document.getElementById("blockTemplate");
const form = document.getElementById("blockForm");
const blockData = document.getElementById("blockData");
const refreshBtn = document.getElementById("refreshBtn");

function setStatus(text, isError = false) {
  statusEl.textContent = text;
  statusEl.style.color = isError ? "#b63a28" : "#1f8f55";
}

function renderChain(blocs = []) {
  chainListEl.innerHTML = "";
  counterEl.textContent = `${blocs.length} bloc${blocs.length > 1 ? "s" : ""}`;

  if (blocs.length === 0) {
    const empty = document.createElement("p");
    empty.textContent = "Aucun bloc pour le moment.";
    chainListEl.appendChild(empty);
    return;
  }

  blocs.forEach((bloc) => {
    const node = template.content.firstElementChild.cloneNode(true);
    node.querySelector(".index").textContent = String(bloc.index);
    node.querySelector(".data").textContent = bloc.data;
    node.querySelector(".nonce").textContent = String(bloc.nonce);
    node.querySelector(".prev").textContent = String(bloc.previous_hash);
    node.querySelector(".hash").textContent = String(bloc.hash);
    chainListEl.appendChild(node);
  });
}

async function chargerChaine() {
  try {
    setStatus("Chargement...");
    const response = await fetch("/api/chain");
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}`);
    }
    const payload = await response.json();
    renderChain(payload.blocs || []);
    setStatus("Chaine a jour");
  } catch (err) {
    setStatus(`Erreur: ${err.message}`, true);
  }
}

async function ajouterBloc(data) {
  try {
    setStatus("Ajout du bloc...");
    const response = await fetch("/api/blocks", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ data }),
    });
    if (!response.ok) {
      const payload = await response.json().catch(() => ({}));
      throw new Error(payload.error || `HTTP ${response.status}`);
    }
    const payload = await response.json();
    renderChain(payload.blocs || []);
    blockData.value = "";
    setStatus("Bloc ajoute et sauvegarde");
  } catch (err) {
    setStatus(`Erreur: ${err.message}`, true);
  }
}

form.addEventListener("submit", (event) => {
  event.preventDefault();
  const data = blockData.value.trim();
  if (!data) {
    setStatus("Ecris un texte avant d'ajouter", true);
    return;
  }
  ajouterBloc(data);
});

refreshBtn.addEventListener("click", chargerChaine);

chargerChaine();
