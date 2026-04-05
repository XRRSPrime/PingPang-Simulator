const messageArea = document.getElementById('simulationResult');

function simulatePingpong(targetScore) {
    const p = Number(document.getElementById('probabilitySlider').value);
    const count = Number(document.getElementById('simulationCount').value);
    if (count < 1000) {
        alert('模拟次数请至少设置为 1000。');
        return;
    }
    let wins = 0;
    for (let i = 0; i < count; i++) {
        let scoreA = 0;
        let scoreB = 0;
        while (true) {
            if (Math.random() < p) {
                scoreA++;
            } else {
                scoreB++;
            }
            const diff = Math.abs(scoreA - scoreB);
            if ((scoreA >= targetScore || scoreB >= targetScore) && diff >= 2) {
                break;
            }
        }
        if (scoreA > scoreB) {
            wins++;
        }
    }
    return wins / count;
}

function showSimulationResult(title, rate) {
    const barWidth = Math.round(rate * 100);
    messageArea.innerHTML = `
        <h4>${title}</h4>
        <p>胜率：<strong>${(rate * 100).toFixed(2)}%</strong></p>
        <div class="bar" style="height: 24px; margin: 12px 0;">
            <div class="bar-fill" style="width: ${barWidth}%; height: 100%;"></div>
        </div>
    `;
}

function compareRules() {
    const rate11 = simulatePingpong(11);
    const rate21 = simulatePingpong(21);
    const diff = ((rate11 - rate21) * 100).toFixed(2);
    messageArea.innerHTML = `
        <h4>11 分制 vs 21 分制</h4>
        <p>11 分制胜率：<strong>${(rate11 * 100).toFixed(2)}%</strong></p>
        <p>21 分制胜率：<strong>${(rate21 * 100).toFixed(2)}%</strong></p>
        <p>差距：<strong>${diff} 个百分点</strong></p>
        <div class="bar" style="height: 18px; margin: 12px 0;">
            <div class="bar-fill" style="width: ${Math.round(rate11 * 100)}%; height: 100%;"></div>
        </div>
        <div class="bar" style="height: 18px; margin-bottom: 12px;">
            <div class="bar-fill" style="width: ${Math.round(rate21 * 100)}%; height: 100%; background: #72bf44;"></div>
        </div>
    `;
}

document.getElementById('probabilitySlider').addEventListener('input', event => {
    document.getElementById('probabilityValue').textContent = Number(event.target.value).toFixed(2);
});

document.getElementById('btnSimulate11').addEventListener('click', () => {
    const rate = simulatePingpong(11);
    showSimulationResult('11 分制模拟结果', rate);
});

document.getElementById('btnSimulate21').addEventListener('click', () => {
    const rate = simulatePingpong(21);
    showSimulationResult('21 分制模拟结果', rate);
});

document.getElementById('btnCompareRules').addEventListener('click', compareRules);