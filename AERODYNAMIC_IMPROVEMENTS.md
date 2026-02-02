# Flight Simulator - Aerodynamic Improvements

## Overview
The aircraft physics have been enhanced to provide more realistic and responsive aerodynamics, putting full control in the user's hands.

## Key Improvements

### 1. **More Realistic Drag Model**
- **Altitude-dependent air density**: Drag now decreases exponentially with altitude using the barometric formula
- **Equipment drag**: Landing gear and flaps now contribute realistic drag penalties
  - Landing gear: +0.025 drag coefficient
  - Each flap level: +0.015 drag coefficient
- **Induced drag**: Increases with angle of attack for more realistic behavior
- **Better scaling**: Drag forces now scale more accurately with speed and air density

### 2. **Enhanced Lift Physics**
- **Improved angle of attack handling**: Lift coefficient now properly increases up to critical angle (~15°) then stalls dramatically
- **Better stall modeling**: Uses Gaussian distribution for realistic stall characteristics
- **Flap assistance**: Flaps increase lift coefficient by 15% per level for slower speed flights
- **Bank angle effects**: Vertical lift component properly accounts for banking

### 3. **User Control Authority**
- **Speed-based control**: Control authority scales with airspeed for realistic handling
- **Pitch control** (improved):
  - Rate multiplier: 4.5x turn rate (up from 3.0x)
  - Range: ±50° (up from ±45°)
  - More direct response to user input
  
- **Roll control** (more responsive):
  - Rate multiplier: 8.0x turn rate (significantly increased)
  - Range: ±90° (full inverted flight capability)
  - Enables barrel rolls and advanced aerobatics
  
- **Yaw control** (improved):
  - Coordinated turn rate: 2.0x turn rate
  - Natural banking: Automatic yaw correlation from roll angle
  - Better rudder authority for precision flying

### 4. **Speed & Thrust Model**
- **More realistic thrust**: Thrust force scales with weight and throttle percentage
- **Continuous drag**: Drag is always applied in flight, creating realistic deceleration
- **Better acceleration**: Separated ground and air acceleration models
- **Overspeed capability**: Aircraft can briefly exceed max speed by 10% (realistic envelope)

### 5. **Vertical Dynamics**
- **Improved vertical speed calculation**: Based on actual lift-weight differential
- **Pitch contribution**: Additional vertical velocity from nose-up/down pitch
- **Climb rate limits**: Properly enforced based on aircraft specs (up to 1.5x for negative)
- **Altitude density**: Thinner air at altitude correctly affects all aerodynamic forces

### 6. **Ground Operations**
- **Responsive steering**: Nose wheel steering scales with speed
- **Smooth leveling**: Aircraft gently returns to level on ground (less aggressive)
- **Better braking**: Brakes now 50% stronger for realistic stopping distance
- **Speed-dependent control**: Steering authority increases with ground speed

## Physics Constants Updated

| Parameter | Old | New | Effect |
|-----------|-----|-----|--------|
| Base drag coefficient | 0.02 | 0.018 | More aerodynamic |
| Landing gear drag | +0.01 | +0.025 | Realistic gear penalty |
| Flap drag per level | +0.005 | +0.015 | Stronger flap effect |
| Pitch control rate | 3.0x | 4.5x | More responsive pitch |
| Roll control rate | 6.5x | 8.0x | Sharper rolls |
| Stall lift reduction | 0.3x | Gaussian | Realistic stall |
| Flap lift bonus | +10% | +15% | Better slow flight |
| Climb rate limit | 2.0x | 1.5x | More realistic |

## Flight Characteristics

### Low Speed Flight
- Flaps are essential for low-speed control
- Stall behavior is more pronounced
- Lift coefficient increases up to 15° angle of attack
- Altitude affects stall speed significantly

### High Speed Flight
- Drag becomes dominant force
- Roll rates are highly responsive
- Pitch authority decreases slightly due to speed
- Landing gear and flaps create significant drag penalty

### Maneuvering
- Aircraft can now perform full aerobatic maneuvers
- Coordinated turns feel natural and responsive
- Bank angles up to 90° enable advanced flying techniques
- Pitch authority scales with airspeed for realistic feel

### Landing
- Gear and flaps provide realistic drag for shorter landing distance
- Stronger braking action improves stopping ability
- Gentle auto-leveling helps with approach stability

## User Control Integration

All improvements maintain **full user control** with:
- Direct keyboard/controller input mapping
- Speed-scaled responsiveness (more control authority at higher speeds)
- Realistic but forgiving physics (no instant snap maneuvers)
- Predictable and consistent aircraft behavior

## Technical Implementation

### File Changes
- **Aircraft.cpp**: Enhanced `applyPhysics()` with improved speed, lift, and drag calculations
- **Physics.cpp**: Refined `calculateLift()` and `calculateDrag()` functions

### Key Equations
- **Drag**: `F_drag = 0.5 * Cd * ρ * v² * A`
- **Lift**: `F_lift = 0.5 * Cl * ρ * v² * A`
- **Air Density**: `ρ = ρ₀ * exp(-altitude / 8500m)`
- **Control Authority**: `CA = min(1.0, speed / (stallSpeed * 1.5))`

## Testing Recommendations

1. **Low Speed Handling**: Fly the Cessna 172 at stall speed
2. **Acrobatics**: Attempt barrel rolls and loops with the F-16
3. **High Altitude Flight**: Climb to max altitude and observe air density effects
4. **Landing Gear Effect**: Compare takeoff distance with gear up vs down
5. **Flap Control**: Practice approaches with various flap settings
