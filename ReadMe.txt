### Setup

1. Navigate to project folder and right-click `RobotTraining.uproject`
   - Select **"Generate Visual Studio project files"**

2. Open `RobotTraining.sln` in Visual Studio or Rider

3. Build the solution:
   - Configuration: **Development Editor**
   - Platform: **Win64**

4. Launch `RobotTraining.uproject` in Unreal Engine

5. In folder "Levels" load the level "MainLevel".

## Controls & Usage

### Camera Controls
- **WASD** - Move camera
- **Hold Left Mouse Button** - Rotate camera

### Interaction
- **Left Click on arm** - Pick up individual arm
- **Left Click on torso** - Pick up entire robot
- **Left Click on attachment sphere** - Attach arm to socket (if compatible side)
- **Left Click elsewhere** - Drop held object at current position

### Mechanics
- Both arms can be detached and moved independently
- Arms are side-specific: left arm only attaches to left socket, right to right socket
- Attachment points display visual spheres when arms are removed
- Attempting to attach to wrong socket type keeps the arm held (no drop)
- Held actor is offset from mouse, code is written to fix this but needs to be configured

## Development Notes

### Unit Testing
Automated unit tests were implemented to validate the attachment system's type compatibility and state transitions. 
Type "Automation RunTests RobotAbuse" into the console in Unreal to test.

The tests validate:
- Left/right arm type validation (left arms only fit left sockets)
- Part state transitions (DETACHED → HELD → ATTACHED)
- Null safety checks



