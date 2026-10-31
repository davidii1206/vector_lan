# Vector — Raylib Prototype TODO

A practical, prototype-sized implementation plan based on the Vector GDD. The goal is to validate the game's defining feel before building the full custom-engine, multiplayer, or content pipeline.

## Prototype target

**First playable milestone:** a single-player, graybox Deathmatch sandbox with one weapon, deterministic movement-based accuracy, responsive FPS movement, one gravity test zone, and one usable ability (Grapple). It should be possible to shoot targets, move around the test map, die/respawn, and tune the important values live.

**Working assumption:** C++ with raylib. Use raylib for windowing, input, rendering, and basic audio. Treat physics, player movement, gravity, weapons, and gameplay rules as game code rather than expecting raylib to provide them.

**Prototype rule:** implement one mechanic at a time in an isolated test room. Do not build the full ability pool, network layer, final maps, or elaborate art before the core feel is proven.

---

## Phase 0 — Project setup and raylib foundation

- [x] Create a C++ project and integrate raylib.
- [x] Set up a clean source layout (`core`, `player`, `weapons`, `abilities`, `world`, `ui`, `debug`).
- [x] Create the application loop: initialize, update, draw, shutdown.
- [x] Add window resizing and a configurable target frame rate / VSync setting.
- [ ] Add mouse capture, cursor toggle, and basic FPS camera look.
- [ ] Add keyboard/mouse input mapping in one place; avoid hard-coding input checks throughout gameplay code.
- [ ] Separate `Update` logic from `Draw` logic.
- [ ] Use a fixed physics/gameplay timestep with render interpolation if needed; keep rendering frame-rate independent.
- [ ] Add a simple debug overlay: FPS, frame time, player position, velocity, movement state, current gravity vector, and current weapon state.
- [ ] Add a small graybox test room made from primitive boxes/planes.
- [ ] Add basic collision for the test room. Choose one approach early:
  - [ ] **Fast prototype:** custom kinematic player controller against simple graybox geometry.
  - [ ] **More robust option:** integrate Jolt if arbitrary gravity and collision response become difficult to maintain.
- [ ] Add a basic `GameConfig` / tuning file for movement, weapon, and ability values.

**Done when:** the project launches reliably, mouse-look works, the player can stand and move in a graybox room, and debug values are visible.

## Phase 1 — Basic player controller

- [ ] Implement player position, velocity, orientation, and grounded state.
- [ ] Implement walking and air movement.
- [ ] Implement jump and landing detection.
- [ ] Implement acceleration, deceleration, friction, and air-control tuning.
- [ ] Add a clear movement-state enum (e.g. grounded, airborne, sliding, wall-sliding).
- [ ] Add collision checks for floors, walls, and ceilings.
- [ ] Add a simple reset/teleport-to-spawn command for testing.
- [ ] Add a test ramp, ledge, low ceiling, and wall to the movement room.

**Done when:** ordinary FPS movement feels stable, collisions are predictable, and the player cannot fall through or get stuck in the basic test geometry.

## Phase 2 — First weapon and deterministic gunplay

Start with the stationary case. Do **not** add decoupled physical gun aiming yet; the GDD explicitly makes that dependent on recoil testing.

- [ ] Implement one rifle archetype with configurable fire rate, damage, range, and recovery.
- [ ] Implement hitscan firing and a visible hit result (impact marker, target response, or debug ray).
- [ ] Implement a target dummy with health and reset behavior.
- [ ] Implement a deterministic accuracy function based on movement state:
  - [ ] Standing still: perfect / tightest accuracy.
  - [ ] Walking: small deterministic spread.
  - [ ] Sliding: larger deterministic spread.
  - [ ] Airborne: define and tune a deterministic penalty.
- [ ] Ensure spread is generated deterministically from shot state/input, not random bloom.
- [ ] Add a firing cooldown and prevent firing faster than the configured rate.
- [ ] Add basic recoil with vertical kick and configurable recovery.
- [ ] Test recoil while stationary before using movement as an input to recoil.
- [ ] Prototype the aim-correction-error → next-recoil-kick relationship:
  - [ ] Track aim correction velocity/direction.
  - [ ] Smooth or deadzone raw input to avoid amplifying sensor jitter.
  - [ ] Add a nonlinear response curve so tiny corrections have minimal effect.
  - [ ] Add a horizontal angle limit to prevent recoil from spiraling sideways.
  - [ ] Display debug vectors/values for correction input and resulting recoil.
- [ ] Compare several recoil settings in a stationary firing range.
- [ ] Add audio/visual feedback for firing, impact, and reload if reload is part of the prototype loop.

**Done when:** identical input produces identical accuracy/recoil results, movement changes accuracy in predictable ways, and the stationary recoil model is understandable and controllable.

## Phase 3 — Movement expression

- [ ] Implement sliding with entry conditions, friction, duration/exit rules, and speed caps.
- [ ] Implement wall sliding with a clear wall-contact condition and controllable descent.
- [ ] Tune the speed ceiling so movement is expressive but does not dominate gun skill.
- [ ] Add movement-state debug indicators and optional velocity arrows.
- [ ] Test accuracy penalties for each movement state against the same targets.
- [ ] Add landing/recovery feedback only where it affects movement readability.
- [ ] Verify players can still aim and fight without using advanced movement.

**Done when:** slide and wall-slide are reliable, have readable limits, and do not make basic aiming/combat feel irrelevant.

## Phase 4 — Variable gravity

- [ ] Represent gravity as a vector and magnitude, not only a world-down scalar.
- [ ] Add gravity-zone volumes to the graybox map.
- [ ] On entering a zone, override the player's local gravity vector/magnitude.
- [ ] Support at least two test cases:
  - [ ] Reduced gravity.
  - [ ] Gravity rotated 90° toward a wall or ceiling.
- [ ] Smoothly reorient the player to the new gravity direction over a configurable 0.3–0.5 seconds.
- [ ] Handle exiting a zone and returning to the previous/default gravity.
- [ ] Keep camera orientation stable and avoid sudden unwanted roll/pitch flips.
- [ ] Add a debug gizmo showing the active gravity direction and magnitude.
- [ ] Compare a directional vignette or other in-world indicator against the gizmo; keep whichever is clearest in motion.
- [ ] Re-test walking, jumping, sliding, wall contact, and weapon accuracy under changed gravity.

**Done when:** the player can enter, traverse, and leave a rotated/reduced-gravity zone without losing control or receiving ambiguous feedback.

## Phase 5 — First ability: Grapple Hook

- [ ] Add an ability interface with activation, cooldown, update, and cancel/reset behavior.
- [ ] Implement grapple targeting against valid surfaces.
- [ ] Draw a visible grapple line/target indicator for debugging.
- [ ] Pull the player toward the target along the current gravity-aware movement rules.
- [ ] Add a meaningful cooldown (not charges).
- [ ] Define behavior when the target is lost, obstructed, or out of range.
- [ ] Add clear activation and cooldown feedback.
- [ ] Test grapple in a flat room, vertical shaft, and gravity-rotated room.
- [ ] Tune it so it creates useful traversal options without becoming mandatory for every fight.

**Done when:** grapple works consistently across the initial gravity cases and its cooldown/range create a readable rhythm.

## Phase 6 — Combat sandbox and Deathmatch test loop

- [ ] Add player health and damage handling.
- [ ] Add a simple death state and respawn flow.
- [ ] Add static or basic moving target dummies; optionally add a very simple bot only after the mechanics are testable without it.
- [ ] Add a small graybox arena with several ranges, cover pieces, a vertical section, and one gravity zone.
- [ ] Add spawn points and a reliable reset flow.
- [ ] Add a minimal HUD: health, weapon, ammo if used, and grapple cooldown.
- [ ] Add a basic Deathmatch score / first-to-N loop only if it helps repeated testing; otherwise keep the sandbox endless.
- [ ] Run repeated tests focused on gunfeel, movement readability, and grapple usefulness.
- [ ] Record tuning notes and adjust one variable group at a time.

**Done when:** the prototype supports repeated combat tests without restarting the application or manually resetting the scene.

## Phase 7 — Additional mechanics (only after the core loop works)

Treat these as experiments, not requirements for the first playable build. Implement each in an isolated test room.

### Air Steps
- [ ] Add sequential midair jumps with a configurable count (GDD target: 3).
- [ ] Reset the count on landing or gravity transition.
- [ ] Add the glowing footprint/trail tell.
- [ ] Test whether the extra jumps create useful options without making movement too dominant.

### Gravity Flip
- [ ] Add a short personal gravity inversion.
- [ ] Add an obvious visual/audio tell and duration indicator.
- [ ] Test on flat floors, near ceilings, and inside gravity zones.
- [ ] Check for disorientation, exploit cases, and interactions with player reorientation.

### Time Slow
- [ ] Implement player-local time scaling; do not change global engine time.
- [ ] Apply it consistently to player movement, relevant animations, and weapon fire cooldown.
- [ ] Reduce the firing rate while active as specified in the GDD.
- [ ] Add opponent-readable audio/animation feedback even in the single-player test (to validate the tell).

### Phase Sight
- [ ] Build a static, read-only voxel/query representation of the graybox geometry.
- [ ] Keep the structure query-only; do not implement world mutation or remeshing.
- [ ] Query nearby thin/qualifying geometry from the player's view when the ability activates.
- [ ] Mark qualifying geometry passable for the player only.
- [ ] Make the player's collision queries ignore marked geometry while active.
- [ ] Make the player's weapon raycasts ignore the same marked geometry.
- [ ] Ensure the world geometry remains solid for all other entities (when/if other entities are added).
- [ ] Add clear duration/state feedback and test edge cases at corners and overlapping surfaces.

### Other abilities and passives
- [ ] Add Gravity Anchor only after basic gravity zones are stable.
- [ ] Add Stasis Field only after projectile behavior is implemented; separately test projectile freeze/release and hitscan reflection.
- [ ] Add Decoy Pulse only after positional audio cues exist.
- [ ] Add Overclock and verify its accuracy tradeoff is meaningful.
- [ ] Add Tether only when teammate/player interaction exists.
- [ ] Add passives one at a time (start with Inertial Dampener and Gravity Acclimation).
- [ ] Test notable combinations only after the individual abilities are understandable.

## Phase 8 — First map built from zone archetypes

- [ ] Build isolated graybox rooms for the zone archetypes before assembling a full map:
  - [ ] The Shaft: tall corridor with multiple platform heights.
  - [ ] The Slab: open space with gravity pulling toward a wall/ceiling.
  - [ ] The Maze: tight corridors and thin geometry.
  - [ ] The Bridge: exposed crossing with drops on both sides.
  - [ ] The Engine Room: mid-range cover and climbable surfaces.
- [ ] Tune corridor widths, cover spacing, heights, and gravity strength in isolation.
- [ ] Combine only the zones needed for the prototype into one compact arena.
- [ ] Ensure each test area offers advantages and disadvantages rather than directly balancing every kit with numerical changes.

**Done when:** the graybox arena creates noticeably different traversal/combat situations and no single movement mechanic is required everywhere.

## Phase 9 — Optional objective-mode proof of concept

Do this only after Deathmatch validates the core gunplay and movement.

- [ ] Add a central Core pickup object.
- [ ] Add carrier state; disable active abilities while carrying, but preserve passives.
- [ ] Add three extraction-point locations and make only one active at a time.
- [ ] Make the active extraction point equally visible/known to both sides (for a later multiplayer build).
- [ ] Drop the Core at the carrier's position on death; allow either side to recover it.
- [ ] Implement first-to-3 extractions and quick round/reset behavior.
- [ ] Keep abilities focused on controlling/repositioning around the Core, not directly modifying the Core.
- [ ] Defer stylish-kill score multipliers.

**Done when:** a complete pickup → carry → contest → extract loop can be played repeatedly in a graybox map.

## Phase 10 — Performance and prototype cleanup

- [ ] Profile CPU and GPU frame time in the largest prototype room.
- [ ] Check performance at the GDD's 120 FPS target on the intended Radeon 680M-class hardware; record actual results rather than assuming the target is met.
- [ ] Reduce unnecessary draw calls, overdraw, and per-frame allocations.
- [ ] Keep debug visualization behind toggles.
- [ ] Add basic save/load of tuning settings so experiments are reproducible.
- [ ] Document known issues, current tuning values, and mechanics that did not pass testing.

**Done when:** the prototype runs consistently, its tuning is reproducible, and there is a clear decision on which mechanics deserve full production implementation.

---

## Explicitly out of scope for the first raylib prototype

- [ ] Steamworks, matchmaking, listen-host networking, replication, prediction, and sub-tick input timestamping.
- [ ] Full ability/passive pool and kit-selection UI.
- [ ] Final art, final animation, final map count, and a production asset-baking pipeline.
- [ ] Custom OpenGL renderer work beyond what raylib requires; use raylib's normal drawing path first.
- [ ] Ray-traced/voxel audio occlusion, unless audio itself becomes the focus of a later experiment.
- [ ] Multiplayer-only fairness and synchronization validation.
- [ ] Host migration and dedicated servers.

These are deferred rather than rejected. Revisit them after the single-player prototype proves the core mechanics.

## Prototype acceptance checklist

- [ ] Launches into a playable graybox FPS scene.
- [ ] Mouse-look, movement, collision, jump, slide, and wall-slide work.
- [ ] At least one gravity zone changes gravity direction or magnitude and reorients the player smoothly.
- [ ] One rifle has deterministic, movement-state-based accuracy with no random bloom.
- [ ] Stationary recoil model is testable and understandable before movement inputs are added to it.
- [ ] Grapple works with a cooldown and is usable in more than one spatial orientation.
- [ ] Targets can be shot, damaged, reset, and used for repeatable tests.
- [ ] The prototype exposes enough debug information to tune movement, gravity, accuracy, and recoil.
- [ ] Performance is measured against the target hardware and documented.
- [ ] A written playtest decision exists for each experimental mechanic: keep, revise, or cut.
