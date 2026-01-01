#pragma once
#include <Arduino.h>

// HTML page stored in PROGMEM to save RAM
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>ESP32 Radar - Distance Monitor</title>
    <style>
      * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
      }

      body {
        font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto,
          Oxygen, Ubuntu, Cantarell, sans-serif;
        background: linear-gradient(
          135deg,
          #1e3c72 0%,
          #2a5298 50%,
          #7e22ce 100%
        );
        background-attachment: fixed;
        min-height: 100vh;
        display: flex;
        justify-content: center;
        align-items: center;
        padding: 10px;
        position: relative;
        overflow-x: hidden;
      }

      body::before {
        content: "";
        position: fixed;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background: radial-gradient(
            circle at 20% 50%,
            rgba(120, 119, 198, 0.3),
            transparent 50%
          ),
          radial-gradient(
            circle at 80% 80%,
            rgba(255, 119, 198, 0.3),
            transparent 50%
          );
        pointer-events: none;
        z-index: 0;
      }

      .container {
        background: rgba(255, 255, 255, 0.95);
        backdrop-filter: blur(20px);
        border-radius: 24px;
        box-shadow: 0 25px 80px rgba(0, 0, 0, 0.4),
          0 0 0 1px rgba(255, 255, 255, 0.1);
        padding: 45px;
        max-width: 600px;
        width: 100%;
        text-align: center;
        position: relative;
        z-index: 1;
        animation: fadeInUp 0.6s ease-out;
        margin: auto;
      }

      @keyframes fadeInUp {
        from {
          opacity: 0;
          transform: translateY(30px);
        }
        to {
          opacity: 1;
          transform: translateY(0);
        }
      }

      h1 {
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        -webkit-background-clip: text;
        -webkit-text-fill-color: transparent;
        background-clip: text;
        margin-bottom: 8px;
        font-size: 2.5em;
        font-weight: 700;
        letter-spacing: -0.5px;
      }

      .subtitle {
        color: #64748b;
        margin-bottom: 35px;
        font-size: 0.95em;
        font-weight: 500;
      }

      .distance-display {
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        white-space: nowrap;
        overflow-x: auto;
        overflow-y: hidden;
        color: white;
        padding: 35px 40px;
        border-radius: 18px;
        font-size: 3.2em;
        font-weight: 700;
        margin: 30px 0;
        min-height: 130px;
        display: flex;
        align-items: center;
        justify-content: center;
        box-shadow: 0 15px 40px rgba(102, 126, 234, 0.5),
          inset 0 1px 0 rgba(255, 255, 255, 0.2);
        transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
        word-break: keep-all;
        position: relative;
        overflow: hidden;
      }

      .distance-display::before {
        content: "";
        position: absolute;
        top: 0;
        left: -100%;
        width: 100%;
        height: 100%;
        background: linear-gradient(
          90deg,
          transparent,
          rgba(255, 255, 255, 0.2),
          transparent
        );
        animation: shimmer 3s infinite;
      }

      @keyframes shimmer {
        0% {
          left: -100%;
        }
        100% {
          left: 100%;
        }
      }

      .distance-display:hover {
        transform: translateY(-3px) scale(1.02);
        box-shadow: 0 20px 50px rgba(102, 126, 234, 0.6),
          inset 0 1px 0 rgba(255, 255, 255, 0.3);
      }

      .status {
        margin-top: 25px;
        padding: 12px 20px;
        border-radius: 12px;
        font-size: 0.9em;
        font-weight: 600;
        display: inline-flex;
        align-items: center;
        gap: 8px;
        transition: all 0.3s ease;
      }

      .status::before {
        content: "";
        width: 8px;
        height: 8px;
        border-radius: 50%;
        display: inline-block;
        animation: pulse 2s infinite;
      }

      @keyframes pulse {
        0%,
        100% {
          opacity: 1;
        }
        50% {
          opacity: 0.5;
        }
      }

      .status.connected {
        background: linear-gradient(135deg, #d4edda 0%, #c3e6cb 100%);
        color: #155724;
        box-shadow: 0 4px 15px rgba(21, 87, 36, 0.2);
      }

      .status.connected::before {
        background: #28a745;
      }

      .status.disconnected {
        background: linear-gradient(135deg, #f8d7da 0%, #f5c6cb 100%);
        color: #721c24;
        box-shadow: 0 4px 15px rgba(114, 28, 36, 0.2);
      }

      .status.disconnected::before {
        background: #dc3545;
        animation: none;
      }

      .status.connecting {
        background: linear-gradient(135deg, #fff3cd 0%, #ffeaa7 100%);
        color: #856404;
        box-shadow: 0 4px 15px rgba(133, 100, 4, 0.2);
      }

      .status.connecting::before {
        background: #ffc107;
      }

      .toggle-button {
        margin-top: 25px;
        padding: 16px 45px;
        font-size: 1.15em;
        font-weight: 600;
        border: none;
        border-radius: 12px;
        cursor: pointer;
        transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
        box-shadow: 0 6px 20px rgba(0, 0, 0, 0.15);
        position: relative;
        overflow: hidden;
      }

      .toggle-button::before {
        content: "";
        position: absolute;
        top: 50%;
        left: 50%;
        width: 0;
        height: 0;
        border-radius: 50%;
        background: rgba(255, 255, 255, 0.3);
        transform: translate(-50%, -50%);
        transition: width 0.6s, height 0.6s;
      }

      .toggle-button:active::before {
        width: 300px;
        height: 300px;
      }

      .toggle-button.active {
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        color: white;
        box-shadow: 0 8px 25px rgba(102, 126, 234, 0.4);
      }

      .toggle-button.inactive {
        background: linear-gradient(135deg, #e2e8f0 0%, #cbd5e1 100%);
        color: #475569;
      }

      .toggle-button:hover {
        transform: translateY(-3px);
        box-shadow: 0 10px 30px rgba(0, 0, 0, 0.25);
      }

      .toggle-button.active:hover {
        box-shadow: 0 12px 35px rgba(102, 126, 234, 0.5);
      }

      .toggle-button:active {
        transform: translateY(-1px);
      }

      .radar-container {
        margin: 35px 0;
        display: flex;
        flex-direction: column;
        align-items: center;
      }

      .radar-canvas {
        background: radial-gradient(
          circle,
          #0a0e27 0%,
          #1a1f3a 50%,
          #0f1419 100%
        );
        border-radius: 50%;
        box-shadow: 0 0 40px rgba(0, 255, 0, 0.4),
          inset 0 0 120px rgba(0, 255, 0, 0.15), 0 0 0 4px rgba(0, 255, 0, 0.3);
        border: 3px solid rgba(0, 255, 0, 0.6);
        transition: all 0.3s ease;
        display: block;
        max-width: 100%;
        height: auto;
        width: 100%;
        aspect-ratio: 1;
      }

      .radar-canvas:hover {
        box-shadow: 0 0 50px rgba(0, 255, 0, 0.5),
          inset 0 0 150px rgba(0, 255, 0, 0.2), 0 0 0 4px rgba(0, 255, 0, 0.4);
      }

      .radar-info {
        margin-top: 20px;
        display: flex;
        gap: 25px;
        flex-wrap: wrap;
        justify-content: center;
      }

      .info-item {
        display: flex;
        flex-direction: column;
        align-items: center;
        background: linear-gradient(135deg, #f8fafc 0%, #f1f5f9 100%);
        padding: 15px 20px;
        border-radius: 12px;
        min-width: 100px;
        box-shadow: 0 4px 15px rgba(0, 0, 0, 0.08);
        transition: all 0.3s ease;
        border: 1px solid rgba(226, 232, 240, 0.8);
      }

      .info-item:hover {
        transform: translateY(-2px);
        box-shadow: 0 6px 20px rgba(0, 0, 0, 0.12);
      }

      .info-label {
        font-size: 0.75em;
        color: #64748b;
        font-weight: 600;
        text-transform: uppercase;
        letter-spacing: 0.5px;
        margin-bottom: 6px;
      }

      .info-value {
        font-weight: 700;
        color: #1e293b;
        font-size: 1.1em;
        margin-top: 2px;
      }

      @media (max-width: 768px) {
        body {
          padding: 10px;
          align-items: flex-start;
          min-height: auto;
        }

        .container {
          padding: 25px 20px;
          max-width: 100%;
          border-radius: 20px;
          margin: 10px 0;
        }

        h1 {
          font-size: 1.8em;
          margin-bottom: 6px;
        }

        .subtitle {
          font-size: 0.85em;
          margin-bottom: 25px;
        }

        .distance-display {
          font-size: 2.2em;
          padding: 25px 15px;
          min-height: 90px;
          margin: 20px 0;
        }

        .radar-container {
          margin: 25px 0;
        }

        .radar-canvas {
          max-width: 100%;
          width: 100%;
        }

        .radar-info {
          gap: 12px;
          margin-top: 15px;
        }

        .info-item {
          min-width: 0;
          flex: 1;
          padding: 12px 10px;
          min-width: 70px;
        }

        .info-label {
          font-size: 0.7em;
        }

        .info-value {
          font-size: 1em;
        }

        .toggle-button {
          padding: 14px 35px;
          font-size: 1em;
          margin-top: 20px;
          width: 100%;
          max-width: 300px;
        }

        .status {
          margin-top: 20px;
          padding: 10px 18px;
          font-size: 0.85em;
        }
      }

      @media (max-width: 480px) {
        body {
          padding: 5px;
        }

        .container {
          padding: 20px 15px;
          border-radius: 16px;
        }

        h1 {
          font-size: 1.5em;
        }

        .subtitle {
          font-size: 0.8em;
          margin-bottom: 20px;
        }

        .distance-display {
          font-size: 1.8em;
          padding: 20px 12px;
          min-height: 80px;
          margin: 15px 0;
        }

        .radar-container {
          margin: 20px 0;
        }

        .radar-info {
          gap: 8px;
          flex-direction: column;
          width: 100%;
        }

        .info-item {
          width: 100%;
          flex-direction: row;
          justify-content: space-between;
          padding: 10px 15px;
        }

        .info-label {
          margin-bottom: 0;
          margin-right: 10px;
        }

        .info-value {
          margin-top: 0;
        }

        .toggle-button {
          padding: 12px 30px;
          font-size: 0.95em;
          width: 100%;
        }

        .status {
          font-size: 0.8em;
          padding: 10px 15px;
        }
      }

      @media (max-width: 360px) {
        .container {
          padding: 15px 12px;
        }

        h1 {
          font-size: 1.3em;
        }

        .distance-display {
          font-size: 1.5em;
          padding: 18px 10px;
          min-height: 70px;
        }
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>ESP32 Radar</h1>
      <p class="subtitle">Real-time Distance Monitoring</p>
      <div class="distance-display" id="distance">---</div>

      <div class="radar-container">
        <canvas id="radarCanvas" class="radar-canvas"></canvas>
        <div class="radar-info">
          <div class="info-item">
            <span class="info-label">Current Angle</span>
            <span class="info-value" id="currentAngle">0°</span>
          </div>
          <div class="info-item">
            <span class="info-label">Max Range</span>
            <span class="info-value">200 cm</span>
          </div>
          <div class="info-item">
            <span class="info-label">Targets</span>
            <span class="info-value" id="targetCount">0</span>
          </div>
        </div>
      </div>

      <button class="toggle-button inactive" id="toggleBtn">
        Start System
      </button>
      <div class="status connecting" id="status">Connecting...</div>
    </div>

    <script>
      let systemActive = false;
      const distanceEl = document.getElementById("distance");
      const statusEl = document.getElementById("status");
      const toggleBtn = document.getElementById("toggleBtn");
      const currentAngleEl = document.getElementById("currentAngle");
      const targetCountEl = document.getElementById("targetCount");
      const ws = new WebSocket(`ws://${location.hostname}:81/`);

      // Radar canvas setup
      const canvas = document.getElementById("radarCanvas");
      const ctx = canvas.getContext("2d");

      const maxRange = 200; // cm

      // Store detected targets with timestamps for fading
      let targets = [];
      let currentAngle = 0;
      let sweepAngle = 0;

      // Make canvas responsive
      function resizeCanvas() {
        const container = canvas.parentElement;
        const containerWidth = container.offsetWidth;
        const padding = 40; // Account for container padding
        const maxSize = Math.min(
          450,
          containerWidth - padding,
          window.innerWidth - padding
        );

        // Set canvas size
        canvas.width = maxSize;
        canvas.height = maxSize;

        // Set CSS size to maintain aspect ratio
        canvas.style.width = maxSize + "px";
        canvas.style.height = maxSize + "px";
      }

      // Initial resize
      resizeCanvas();

      // Resize on window resize with debounce
      let resizeTimeout;
      window.addEventListener("resize", () => {
        clearTimeout(resizeTimeout);
        resizeTimeout = setTimeout(() => {
          resizeCanvas();
        }, 100);
      });

      // Also resize when orientation changes
      window.addEventListener("orientationchange", () => {
        setTimeout(() => {
          resizeCanvas();
        }, 200);
      });

      function updateButtonState(active) {
        systemActive = active;
        if (active) {
          toggleBtn.textContent = "Stop System";
          toggleBtn.className = "toggle-button active";
        } else {
          toggleBtn.textContent = "Start System";
          toggleBtn.className = "toggle-button inactive";
          // Reset distance display when system becomes inactive
          distanceEl.textContent = "---";
          // Clear targets when system stops
          targets = [];
          targetCountEl.textContent = "0";
        }
      }

      function drawRadar() {
        // Update canvas dimensions if resized
        const currentCenterX = canvas.width / 2;
        const currentCenterY = canvas.height / 2;
        const currentMaxRadius = Math.min(currentCenterX, currentCenterY) - 25;

        // Clear canvas with slight fade for sweep trail effect
        ctx.fillStyle = "rgba(10, 14, 39, 0.08)";
        ctx.fillRect(0, 0, canvas.width, canvas.height);

        // Draw concentric circles (distance rings)
        ctx.strokeStyle = "rgba(0, 255, 0, 0.35)";
        ctx.lineWidth = 1.5;
        for (let i = 1; i <= 4; i++) {
          const radius = (currentMaxRadius * i) / 4;
          ctx.beginPath();
          ctx.arc(currentCenterX, currentCenterY, radius, 0, Math.PI * 2);
          ctx.stroke();
        }

        // Draw angle lines (0°, 45°, 90°, 135°, 180°)
        ctx.strokeStyle = "rgba(0, 255, 0, 0.25)";
        ctx.lineWidth = 1;
        const angles = [0, 45, 90, 135, 180];
        angles.forEach((angle) => {
          const rad = ((angle - 90) * Math.PI) / 180;
          ctx.beginPath();
          ctx.moveTo(currentCenterX, currentCenterY);
          ctx.lineTo(
            currentCenterX + Math.cos(rad) * currentMaxRadius,
            currentCenterY + Math.sin(rad) * currentMaxRadius
          );
          ctx.stroke();
        });

        // Draw angle labels (responsive font size)
        const fontSize = Math.max(10, Math.min(13, currentMaxRadius / 20));
        ctx.font = `bold ${fontSize}px monospace`;
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        angles.forEach((angle) => {
          const rad = ((angle - 90) * Math.PI) / 180;
          const labelOffset =
            currentMaxRadius + Math.max(12, currentMaxRadius / 20);
          const labelX = currentCenterX + Math.cos(rad) * labelOffset;
          const labelY = currentCenterY + Math.sin(rad) * labelOffset;
          // Add background for better readability
          const labelWidth = fontSize * 2.5;
          const labelHeight = fontSize * 1.5;
          ctx.fillStyle = "rgba(10, 14, 39, 0.7)";
          ctx.fillRect(
            labelX - labelWidth / 2,
            labelY - labelHeight / 2,
            labelWidth,
            labelHeight
          );
          ctx.fillStyle = "rgba(0, 255, 0, 0.9)";
          ctx.fillText(angle + "°", labelX, labelY);
        });

        // Draw distance labels (responsive font size)
        const distanceFontSize = Math.max(
          8,
          Math.min(11, currentMaxRadius / 25)
        );
        ctx.font = `bold ${distanceFontSize}px monospace`;
        ctx.textAlign = "left";
        ctx.textBaseline = "middle";
        for (let i = 1; i <= 4; i++) {
          const radius = (currentMaxRadius * i) / 4;
          const distance = (maxRange * i) / 4;
          const labelX =
            currentCenterX + radius - Math.max(20, currentMaxRadius / 15);
          const labelY = currentCenterY - 8;
          const labelWidth = distanceFontSize * 3.5;
          const labelHeight = distanceFontSize * 1.5;
          // Add background for better readability
          ctx.fillStyle = "rgba(10, 14, 39, 0.7)";
          ctx.fillRect(
            labelX - 2,
            labelY - labelHeight / 2,
            labelWidth,
            labelHeight
          );
          ctx.fillStyle = "rgba(0, 255, 0, 0.8)";
          ctx.fillText(distance + "cm", labelX, labelY);
        }

        // Draw sweep line
        if (systemActive) {
          const sweepRad = ((sweepAngle - 90) * Math.PI) / 180;

          // Draw sweep arc (trail) - wider and more visible
          ctx.strokeStyle = "rgba(0, 255, 0, 0.15)";
          ctx.lineWidth = 25;
          ctx.beginPath();
          ctx.arc(
            currentCenterX,
            currentCenterY,
            currentMaxRadius,
            sweepRad - 0.15,
            sweepRad + 0.15
          );
          ctx.stroke();

          // Draw main sweep line with gradient effect
          const gradient = ctx.createLinearGradient(
            currentCenterX,
            currentCenterY,
            currentCenterX + Math.cos(sweepRad) * currentMaxRadius,
            currentCenterY + Math.sin(sweepRad) * currentMaxRadius
          );
          gradient.addColorStop(0, "rgba(0, 255, 0, 1)");
          gradient.addColorStop(0.7, "rgba(0, 255, 0, 0.8)");
          gradient.addColorStop(1, "rgba(0, 255, 0, 0.3)");

          ctx.strokeStyle = gradient;
          ctx.lineWidth = 2.5;
          ctx.beginPath();
          ctx.moveTo(currentCenterX, currentCenterY);
          ctx.lineTo(
            currentCenterX + Math.cos(sweepRad) * currentMaxRadius,
            currentCenterY + Math.sin(sweepRad) * currentMaxRadius
          );
          ctx.stroke();
        }

        // Draw detected targets
        const now = Date.now();
        targets = targets.filter((target) => {
          const age = now - target.timestamp;
          const fadeTime = 5000; // 5 seconds fade time
          const opacity = Math.max(0, 1 - age / fadeTime);

          if (opacity <= 0) return false;

          const angleRad = ((target.angle - 90) * Math.PI) / 180;
          const distanceRatio = Math.min(target.distance / maxRange, 1);
          const radius = distanceRatio * currentMaxRadius;

          const x = currentCenterX + Math.cos(angleRad) * radius;
          const y = currentCenterY + Math.sin(angleRad) * radius;

          // Draw target ring (outer glow)
          const ringGradient = ctx.createRadialGradient(x, y, 0, x, y, 10);
          ringGradient.addColorStop(0, `rgba(255, 0, 0, ${opacity * 0.3})`);
          ringGradient.addColorStop(1, `rgba(255, 0, 0, 0)`);
          ctx.fillStyle = ringGradient;
          ctx.beginPath();
          ctx.arc(x, y, 10, 0, Math.PI * 2);
          ctx.fill();

          // Draw target ring
          ctx.strokeStyle = `rgba(255, 0, 0, ${opacity * 0.6})`;
          ctx.lineWidth = 1.5;
          ctx.beginPath();
          ctx.arc(x, y, 9, 0, Math.PI * 2);
          ctx.stroke();

          // Draw target blip (bright center)
          const blipGradient = ctx.createRadialGradient(x, y, 0, x, y, 5);
          blipGradient.addColorStop(0, `rgba(255, 100, 100, ${opacity})`);
          blipGradient.addColorStop(1, `rgba(255, 0, 0, ${opacity * 0.7})`);
          ctx.fillStyle = blipGradient;
          ctx.beginPath();
          ctx.arc(x, y, 5, 0, Math.PI * 2);
          ctx.fill();

          // Draw bright center dot
          ctx.fillStyle = `rgba(255, 255, 255, ${opacity})`;
          ctx.beginPath();
          ctx.arc(x, y, 2, 0, Math.PI * 2);
          ctx.fill();

          return true;
        });

        // Update target count
        targetCountEl.textContent = targets.length;

        // Center dot with glow
        const centerGradient = ctx.createRadialGradient(
          currentCenterX,
          currentCenterY,
          0,
          currentCenterX,
          currentCenterY,
          6
        );
        centerGradient.addColorStop(0, "rgba(0, 255, 0, 1)");
        centerGradient.addColorStop(0.5, "rgba(0, 255, 0, 0.6)");
        centerGradient.addColorStop(1, "rgba(0, 255, 0, 0)");
        ctx.fillStyle = centerGradient;
        ctx.beginPath();
        ctx.arc(currentCenterX, currentCenterY, 6, 0, Math.PI * 2);
        ctx.fill();

        // Center dot core
        ctx.fillStyle = "rgba(0, 255, 0, 0.9)";
        ctx.beginPath();
        ctx.arc(currentCenterX, currentCenterY, 3, 0, Math.PI * 2);
        ctx.fill();
      }

      // Animation loop
      function animate() {
        drawRadar();
        requestAnimationFrame(animate);
      }
      animate();

      toggleBtn.addEventListener("click", () => {
        ws.send(JSON.stringify({ command: "toggle" }));
      });

      ws.onopen = () => {
        statusEl.textContent = "Connected";
        statusEl.className = "status connected";
        // State will be sent automatically by server on connection
      };

      ws.onclose = () => {
        statusEl.textContent = "Disconnected";
        statusEl.className = "status disconnected";
      };

      ws.onerror = () => {
        statusEl.textContent = "Connection Error";
        statusEl.className = "status disconnected";
      };

      ws.onmessage = (e) => {
        try {
          const data = JSON.parse(e.data);
          if (data.type === "state") {
            updateButtonState(data.active);
          } else if (data.type === "distance") {
            distanceEl.textContent = data.value;

            // Update radar with new reading
            if (data.angle !== undefined && data.distance !== undefined) {
              currentAngle = data.angle;
              sweepAngle = data.angle;
              currentAngleEl.textContent = Math.round(data.angle) + "°";

              // Add target if valid reading
              if (data.distance > 0 && data.distance <= maxRange) {
                targets.push({
                  angle: data.angle,
                  distance: data.distance,
                  timestamp: Date.now(),
                });
              }
            }
          }
        } catch (err) {
          // Fallback: treat as plain distance string
          distanceEl.textContent = e.data;
        }
      };
    </script>
  </body>
</html>
)rawliteral";

