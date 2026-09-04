# GuidePlay — A Gamified Haptic Feedback System for Orientation and Mobility Technique Training

> **Team project** — Bao Do, Anudamodaran Kumarakurup, Tejas Mathur Sriganesh.
> Advanced Mechatronics (ROB-GY 6103), New York University.
>
> This repository is a copy published under my own account for portfolio purposes.
> The original upload lives at [baodo0710/GuidePlay](https://github.com/baodo0710/GuidePlay)
> and remains the canonical source.


GuidePlay is an instrumented white cane system integrating inertial measurement, ultrasonic proximity sensing, and multi-channel vibrotactile feedback to transform orientation and mobility (O&M) technique practice into a real-time, screen-free haptic training paradigm. Rather than automating navigation, the system quantifies and grades fundamental cane mechanics — sweep arc width, tap rhythm, wall proximity maintenance, and ground contact consistency — returning performance feedback exclusively through patterned vibration. No visual display, no numerical readout, no audio interrupting the physical practice loop.

**Members:** Bao Do, Anudamodaran Kumarakurup, Tejas Mathur Sriganesh

---

## Motivation and Problem Statement

Contemporary smart cane research predominantly focuses on **obstacle detection and routing** via audio alerts — yet qualitative interviews with cane users consistently identify a critical limitation: audio-based guidance induces **continuous partial attention**, producing cognitive fatigue and interfering with the spatial memory formation that proficient cane technique depends upon. Furthermore, no commercially available system provides **quantitative, real-time assessment of cane mechanics** (arc width, temporal consistency, tip contact, lateral wall clearance), and conventional O&M training gyms require specialized staff and dedicated physical space, severely constraining practice frequency for learners.

GuidePlay's methodological contribution is a **technique-centric training paradigm** rather than a navigation-replacement system. All performance feedback is delivered through the haptic channel, preserving the auditory spatial mapping essential to blind navigation. The sensor package augments a standard white cane without altering its mechanical properties, ensuring ecological validity for the target population.

---

## System Architecture and Interaction Design

### Mode and Difficulty Selection via Kinematic Gesture Recognition

- **No physical buttons.** The operator selects a game mode and difficulty level through a **wrist-twist gesture** — the IMU gyroscope integrates angular velocity to detect rotational displacement, and a ~1-second static hold confirms the selection through a confirmation haptic pulse.
- **Session structure.** Upon confirmation, the system executes a 3-second calibration routine (averaging IMU readings to establish a player-specific baseline for grip angle and mount orientation), followed by a 30-second scoring window comprising 300 discrete 100 ms evaluation intervals.
- **Silence-as-correct feedback policy.** When the operator's technique falls within the calibrated tolerance band, no haptic output is generated. Deviation outside tolerance triggers a spatially specific motor and temporal pattern, enabling **error-type identification without visual or auditory diversion**.
- **Session summary encoding.** At session conclusion: three pulses indicate acceptable performance (≥70% of intervals within tolerance), two pulses indicate partial proficiency (50–69%), and one pulse indicates the technique requires additional practice (<50%).

---

## Firmware-Only Training Modes

These modes execute entirely on the Propeller Activity Board embedded microcontroller — no external computational dependency.

| Mode | Target Skill | Sensing Architecture | Haptic Feedback Mapping |
|---|---|---|---|
| **Sweeping** | Consistent arc width and temporal regularity in the side-to-side cane sweep | IMU gyroscope — peak angular rate per half-sweep (arc amplitude), zero-crossing interval timing (tempo) | Lateral motor activation indicates insufficient arc width on the corresponding side; anterior motor indicates off-tempo deviation |
| **Touch Rhythm** | Rhythmic 2-point or 3-point tapping technique | IMU accelerometer (impact transient detection), front ultrasonic sensor (landmark proximity probe at Intermediate/Advanced difficulty) | Corresponding motor confirms correct on-beat tap; anterior motor warns on missed or off-rhythm beat |
| **Shorelining** | Maintaining consistent lateral distance from a wall or corridor boundary | Left ultrasonic sensor (right sensor engaged at Advanced difficulty) | Left motor encodes drift direction — pulse frequency indicates toward/away deviation from the target clearance |
| **Ground Reading** | Continuous tip-ground contact; curb and step detection; touch-slide gesture at Intermediate/Advanced | IMU accelerometer Z-axis (lift-off detection), front ultrasonic (curb edge detection), IMU accelerometer/gyroscope fusion (touch-slide kinematic signature) | All three motors generate stutter pattern on lift event; anterior motor warns on detected curb or elevation discontinuity |

---

## Computer-Vision-Augmented Spatial Memory Modes

These modes augment the embedded system with a ceiling-mounted USB camera, a laptop-based Python/OpenCV game engine, and Wi-Fi HTTP communication to the Propeller for boundary-warning haptics. The operator wears an earpiece for audio game cues; a display shows a live augmented-reality overlay for observer monitoring. Currently prototyped as a separate software module (see [Current Status](#current-status)).

- **Memory Run** — The operator learns a checkpoint route from rising-pitch audio tones, then retraces it from memory with haptic feedback suppressed. Path deviation is scored via ceiling-camera tracking. At the highest difficulty level, even boundary-warning haptics are disabled, isolating pure spatial memory performance.
- **Cartographer** — The operator freely explores a bounded space to construct a mental map of object locations (each discovered via a unique audio tone), then undergoes a cued-recall quiz on object positions. The spatial layout is pseudo-randomized on every run to prevent memorization-based learning.

Cane position is tracked via **HSV color thresholding** on a high-visibility tape marker. This approach was selected over ArUco fiducial markers, which require a minimum printed size for reliable detection at ceiling-mounting heights and introduce mechanical fragility in a public-use context.

---

## Hardware Platform

- **Parallax Propeller Activity Board** — multi-core embedded microcontroller
- **MPU-6050 Inertial Measurement Unit** (I2C) — mounted below the handle for optimal proximity to the center of rotation
- **Three HC-SR04 ultrasonic rangefinders** — mounted 3–4 inches from the cane tip in anterior, left, and right configurations
- **Three mini vibration motor discs** embedded in the grip assembly, each driven through a 2N2222 NPN transistor with 1 kΩ base current limiting
- **High-visibility color-tape marker** — for computer-vision tracking in the augmented modes

### Pin Assignments (from `firmware/config.h`)

| Pin | Signal | Component |
|---|---|---|
| P0 | MOT_L | Left vibration motor |
| P1 | MOT_R | Right vibration motor |
| P2 | MOT_F | Front vibration motor |
| P3 / P4 | US_FRONT trig/echo | Front ultrasonic |
| P5 / P6 | US_LEFT trig/echo | Left ultrasonic |
| P7 / P8 | US_RIGHT trig/echo | Right ultrasonic |
| P9 / P10 | I2C SCL/SDA | IMU |
| P14–P17 | Buttons (mode up/down, diff, start) | Reserved for future hardware revision; currently superseded by wrist-twist UI |

---

## Repository Structure

```
guideplay/
├── firmware/
│   ├── Pathfinder.c       # Entry point: system initialization, mode/difficulty selection, calibration, session dispatch
│   ├── Pathfinder.side    # SimpleIDE project file (Activity Board, C99, CMM memory model)
│   ├── config.h           # Pin mapping, performance thresholds, difficulty parameter tables, mode/buzz identifiers
│   ├── imu.c / imu.h      # I2C driver for MPU-6050, zero-velocity calibration, 6-axis data acquisition
│   ├── ultrasonic.c/.h    # CNT-timed trigger/echo distance measurement with timeout handling
│   ├── motors.c / motors.h# Vibration motor actuation with parameterized temporal patterns
│   └── modes.c / modes.h  # Four firmware-only training modes + unified session dispatcher
└── docs/
    ├── GuidePlay_Technical_Report.docx   # Complete technical report
    └── GuidePlay_Presentation.pptx      # Project presentation
```

---

## Firmware Build and Deployment

The embedded software targets the **Parallax Propeller** on an **Activity Board**, compiled with **SimpleIDE**.

1. Install [SimpleIDE](https://www.parallax.com/simpleide) (or use `propeller-gcc` / `loadp2` directly).
2. Open `firmware/Pathfinder.side` — build settings are preconfigured:
   - Board: Activity Board
   - Compiler: C, `-std=c99`
   - Memory model: CMM (compact)
   - Optimization: `-Os`
   - Links: `-lm`, `-m32bit-doubles`, `-fno-exceptions`
3. Build and load to RAM or EEPROM via SimpleIDE.

---

## Current Status and Known Limitations

- The four firmware-only modes (Sweeping, Touch Rhythm, Shorelining, Ground Reading) are **fully implemented and hardware-validated**.
- Memory Run and Cartographer were **prototyped and functionally tested** (3–7 checkpoints / objects per session). 
- Performance threshold constants (lift detection, tap impact, arc intensity, etc.) were calibrated against **sighted participants operating under blindfold conditions** and remain pending validation with visually impaired operators before they can be considered ecologically valid.

---

## Acknowledgments

Early conceptual direction for this project emerged from conversations with **Amir Ghahremani** at the Central Association for Blind and Visually Impaired (CABVI), and **Dr. Kristina Boylan** at SUNY Polytechnic Institute, who shared firsthand expertise in white cane use, O&M training methodology, and the systemic shortcomings of existing smart cane designs. Their input directly informed the project's central design decision to focus on **technique training and haptic feedback** rather than navigation automation.
