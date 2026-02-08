# Castle Defense - Gacha System

## Status

This document is the full design reference.  
Implementation should start with the **V1 subset** defined below.

## Overview

- After each level, player picks **1 of 3** random choices (like Vampire Survivors)
- Choices are a mix of **warriors**, **items**, and **upgrades** for already-owned warriors/items
- Base warriors and items have **no rarity tiers** -- all roughly equal power
- Once you own a warrior or item, **upgrades for it enter the gacha pool**
- Upgrades have rarity tiers: **Common / Rare / Epic / Legendary**
- Upgrades are **infinitely repeatable** -- you can keep stacking them
- Castle has **5 warrior slots** and **5 item slots**
- Slots can be rebalanced to **4/6 or 6/4** via one specific warrior (Jeweler) and one specific item (Commander's Seal), but no further -- 4/6 is the limit in either direction

---

## V1 Implementation Rules (Balanced Subset)

Use these rules for the first playable version. Keep the rest of this file as future expansion content.

### 1. Scope
- Implement only the warriors/items listed in **V1 Content Subset**
- Ignore all other entries for now
- Limit mechanics to: projectile hit, AoE, DoT, slow, basic support auras, castle HP/regen/resistance, simple gacha reroll

### 2. Gacha Flow (V1)
- Between levels, player sees **3 choices and picks 1**
- Pool contains:
  - Not-owned base warriors/items that still fit available slots
  - Upgrades for owned warriors/items
- If all 3 rolled options are invalid (slot/full constraints), reroll internally until at least 1 valid option exists

### 3. Rarity and Roll Safety
- Base rarity weights (before Luck):  
  - Common 60%  
  - Rare 28%  
  - Epic 10%  
  - Legendary 2%
- **Guaranteed floor:** every 4th gacha must contain at least one Rare+
- **Legendary lock:** no Legendary results before level 6

### 4. Stacking Caps (Global)
- Attack speed bonus from all sources combined: **cap +200%**
- Cooldown reduction from all sources combined: **cap 70%**
- Global damage amplification from support/item auras: **cap +250%**
- Proc chance on any effect: **cap 75%**
- Damage resistance: **cap 75%**
- Most numeric upgrades: soft diminishing returns after 10 stacks

### 5. Slot Rules (V1)
- Start at **5 warrior / 5 item**
- For V1, disable slot-conversion entities (Jeweler + Commander's Seal) to reduce edge-case logic
- Re-enable in V2 after baseline balance passes

### 6. Engineering Constraints (V1)
- No airborne state system
- No fall damage
- No enemy mind-control/conversion
- No recursive proc chains (effect triggers effect triggers effect)
- No "on-death mass resurrection" mechanics
- Avoid per-frame expensive global scans; use event-driven triggers where possible

---

## V1 Content Subset (Implement First)

### Warriors (8)
1. Fire Archer
2. Ice Archer
3. Lightning Archer
4. Ballista Operator
5. Catapult Engineer
6. Bard
7. Priestess
8. Blacksmith

### Items (8)
1. Ruby Amulet
2. Sapphire Amulet
3. Topaz Amulet
4. Iron Shield Charm
5. Healing Totem
6. Scholar's Tome
7. Lucky Clover
8. Elemental Prism

---

## V1 Upgrade Policy (Per Entity)

- Keep **4 upgrade lines per entity** in V1:
  - 2 Common
  - 1 Rare
  - 1 Epic
- Disable Legendary upgrades entirely in V1
- Keep upgrade text and IDs stable so disabled upgrades can be re-enabled later without save breakage

Suggested V1 pattern:
- Common A: flat damage/effect value
- Common B: speed/duration/range
- Rare: utility or conditional power
- Epic: signature mechanic for that entity

---

## V1 Balance Targets

- Average level clear time target: **45-75s**
- Castle HP loss in a successful run:
  - Early levels: 0-10%
  - Mid levels: 10-35%
  - Late levels (before fail): 35-70%
- A single support pick should not outperform a full damage pick by itself
- "Best build" should be at most ~2x stronger than median random build (not 5x+)

---

## V2 Re-introduction Plan

After V1 is stable:
1. Add 4 more warriors + 4 more items
2. Enable Legendary at low rate
3. Re-enable Jeweler and Commander's Seal
4. Add one advanced mechanic family at a time (airborne/fall OR summon chains OR conversion), never multiple in same patch

## Gacha Pool Rules

1. **Initial pool**: All 20 base warriors + all 20 base items
2. **After picking Fire Archer**: Fire Archer's upgrade list enters the pool
3. **Each roll**: Pick 3 random choices from the pool, player picks 1
4. **Rarity weighting** (affected by Luck stat):
   - Common: 55%
   - Rare: 28%
   - Epic: 12%
   - Legendary: 5%
5. **Full slots**: Don't offer new warriors if warrior slots are full (still offer warrior upgrades). Same for items.
6. **All base picks are limit one**: You can never own duplicates. Once you own a warrior/item, it leaves the base pool and only its upgrades appear
7. **Upgrade stacking**: Most upgrades stack infinitely. Some have natural soft caps (e.g. attack speed has diminishing returns). Legendary upgrades that add mechanics (like extra arrows) stack to add more each time.
8. **Slot swaps**: Jeweler (warrior) gives +1 item slot. Commander's Seal (item) gives +1 warrior slot. Each can only be taken once. They allow 4/6 or 6/4 splits but not both (taking both returns you to 5/5). No further slot expansion exists anywhere in the upgrade system.

---

## Warriors (20)

5 base castle slots. Warriors auto-attack from the castle.

---

### 1. Fire Archer
Shoots flaming arrows. Enemies hit burn for 3 dmg/sec over 3 seconds.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +1 arrow damage | Yes |
| Common | +0.5s burn duration | Yes |
| Rare | +2 burn damage per tick | Yes |
| Rare | +15% attack speed | Yes |
| Epic | Burning enemies spread fire to adjacent enemies | First gives effect, repeats increase spread radius |
| Epic | Arrows explode on impact dealing 50% dmg in small AoE | Repeats increase AoE radius |
| Legendary | Shoot +1 extra arrow per attack | Yes (2, 3, 4... arrows) |

---

### 2. Ice Archer
Shoots frost arrows. Enemies hit are slowed 30% for 2 seconds.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +1 arrow damage | Yes |
| Common | +0.3s slow duration | Yes |
| Rare | +10% slow strength | Yes (caps at 90%) |
| Rare | +15% attack speed | Yes |
| Epic | Slowed enemies take 20% more damage from all sources | Repeats increase bonus by +10% |
| Epic | Chance to freeze enemy solid for 1s (stun) | Repeats increase freeze chance |
| Legendary | Arrows shatter into 3 ice shards on hit, hitting nearby enemies | Repeats add +1 shard |

---

### 3. Lightning Archer
Electric arrows chain to 2 nearby enemies at 50% damage.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +1 arrow damage | Yes |
| Common | +1 chain range (pixels) | Yes |
| Rare | Chain hits +1 extra enemy | Yes |
| Rare | +15% attack speed | Yes |
| Epic | Chain damage increased to 75% (instead of decaying) | Repeats add +10% |
| Epic | Chained enemies are briefly stunned (0.3s) | Repeats add +0.2s stun |
| Legendary | Lightning strikes all enemies in a line from archer to target | Repeats increase line width |

---

### 4. Poison Archer
Venom arrows apply stacking poison. 1 dmg/sec per stack, max 5 stacks.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +1 arrow damage | Yes |
| Common | +0.5 dmg per poison tick | Yes |
| Rare | +2 max poison stacks | Yes |
| Rare | +15% attack speed | Yes |
| Epic | Poison slows enemy by 5% per stack | Repeats add +3% per stack |
| Epic | Enemies that die while poisoned explode in a toxic cloud | Repeats increase cloud radius |
| Legendary | Each arrow applies 2 poison stacks instead of 1 | Repeats add +1 stacks applied |

---

### 5. Wind Archer
Gust arrows knock enemies back and briefly interrupt their attacks.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +1 arrow damage | Yes |
| Common | +10% knockback distance | Yes |
| Rare | Knockback interrupts enemy attacks for +0.5s | Yes |
| Rare | +15% attack speed | Yes |
| Epic | Knocked-back enemies collide with others, dealing damage | Repeats increase collision damage |
| Epic | Arrows create a wind gust that pushes all enemies in a line | Repeats increase gust width |
| Legendary | Arrows launch enemies into the air (enables fall damage!) | Repeats increase launch height |

---

### 6. Ballista Operator
Very slow attack (4s cooldown). Massive single-target damage. Prioritizes strongest enemy.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +5 bolt damage | Yes |
| Common | -0.1s cooldown (min 1.5s) | Yes |
| Rare | Bolts pierce through first target to hit one behind | Repeats add +1 pierce |
| Rare | +25% damage vs enemies above 50% HP | Repeats add +15% |
| Epic | Bolts pin enemies to the ground, stunning for 2s | Repeats add +0.5s stun |
| Epic | Critical hit chance: 15% for double damage | Repeats add +10% crit chance |
| Legendary | Fires 2 bolts per shot | Yes (3, 4...) |

---

### 7. Catapult Engineer
Launches boulders at enemy clusters. Medium speed, AoE damage.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +2 boulder damage | Yes |
| Common | +10% AoE radius | Yes |
| Rare | -0.2s cooldown | Yes |
| Rare | Boulders leave rubble that slows enemies walking over it | Repeats increase slow strength |
| Epic | Boulders shatter into fragments dealing 30% damage in wider area | Repeats increase fragment damage |
| Epic | Targets location with most enemies instead of random | First gives smart targeting, repeats reduce retarget time |
| Legendary | Launch 2 boulders per attack | Yes (3, 4...) |

---

### 8. Bomber
Throws explosive barrels. AoE fire damage on detonation.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +2 explosion damage | Yes |
| Common | +10% explosion radius | Yes |
| Rare | -0.3s throw cooldown | Yes |
| Rare | Barrels leave burning ground for 2s | Repeats add +1s burn duration |
| Epic | Enemies killed by explosion drop a mini-bomb | Repeats increase mini-bomb damage |
| Epic | Barrel bounces once before detonating, hitting two areas | Repeats add +1 bounce |
| Legendary | Throws a mega-barrel: 3x radius, 2x damage (every 3rd attack) | Repeats increase mega frequency |

---

### 9. Dragon
Breathes fire in random ground patches. Large AoE, periodic attack.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +2 fire patch damage | Yes |
| Common | +0.5s fire patch duration | Yes |
| Rare | +1 fire patch per breath | Yes |
| Rare | Fire patches are larger | Yes (+10% radius each) |
| Epic | Enemies that walk through fire have reduced attack damage for 3s | Repeats increase debuff strength |
| Epic | Dragon swoops down to claw nearest enemy between breaths | Repeats increase claw damage |
| Legendary | Fire patches erupt into pillars, launching enemies airborne | Repeats increase launch height |

---

### 10. Eagle
Swoops at airborne/thrown enemies. Bonus damage to falling targets.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +3 swoop damage | Yes |
| Common | +10% bonus vs airborne | Yes |
| Rare | Eagle attacks grounded enemies between swoops (lower damage) | Repeats increase ground attack damage |
| Rare | Swoop knocks enemy further into the air (extends airtime) | Yes |
| Epic | Eagle grabs and drops small enemies for guaranteed fall damage | Repeats allow grabbing larger enemies |
| Epic | Swoop creates a wind gust that pushes nearby grounded enemies | Repeats increase gust range |
| Legendary | Summon +1 additional eagle | Yes (2, 3... eagles) |

---

### 11. Wolf
Sprints out from castle to bite nearest enemy, then returns. Fast cycle.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +2 bite damage | Yes |
| Common | +10% run speed | Yes |
| Rare | Wolf bites twice before returning | Repeats add +1 bite |
| Rare | Bite applies bleed (2 dmg/sec for 3s) | Repeats increase bleed damage |
| Epic | Wolf tackles enemy, knocking them backward | Repeats increase knockback distance |
| Epic | Wolf howl on return: all warriors gain +10% attack speed for 3s | Repeats increase buff duration |
| Legendary | Summon +1 additional wolf | Yes (forms a pack) |

---

### 12. Bard
Support. All other warriors attack 15% faster. Does not attack.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +3% attack speed bonus | Yes |
| Common | Bard's aura range +10% | Yes |
| Rare | Also grants +5% damage to all warriors | Yes |
| Rare | Song reduces enemy movement speed by 5% | Yes |
| Epic | Inspire: random warrior fires a bonus attack every 3s | Repeats reduce interval |
| Epic | War drum: enemies near castle are intimidated, -15% attack speed | Repeats increase debuff |
| Legendary | Bard's buffs are doubled for 5s every 15s (crescendo) | Repeats increase crescendo duration |

---

### 13. Priestess
Support. Castle heals 2 HP/sec passively.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +0.5 HP/sec healing | Yes |
| Common | +5 max castle HP | Yes |
| Rare | Heal burst: restore 10 HP when a wave of 3+ enemies die within 2s | Repeats add +5 HP burst |
| Rare | Barrier: absorb next 5 damage every 10s | Repeats add +3 absorb |
| Epic | Resurrection: once per level, restore castle to 30% HP when it would hit 0 | Repeats add +10% restored |
| Epic | Holy ground: enemies near castle deal 15% less damage | Repeats increase reduction |
| Legendary | Divine shield: castle is invulnerable for 3s every 20s | Repeats add +1s duration |

---

### 14. Blacksmith
Support. All warrior damage +20%. Does not attack directly.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +5% warrior damage bonus | Yes |
| Common | Warriors have +5% chance to critically hit | Yes |
| Rare | Forged weapons: warriors deal +2 flat bonus damage per hit | Yes |
| Rare | Armor piercing: warrior attacks ignore 10% of enemy toughness | Yes |
| Epic | Flaming forge: all warrior attacks gain a small fire DoT | Repeats increase DoT |
| Epic | Master craft: one random warrior gains +50% damage each level | Repeats affect +1 additional warrior |
| Legendary | Legendary weapon: all warrior attacks deal double damage every 5th hit | Repeats reduce hit interval (4th, 3rd...) |

---

### 15. Watchman
Support. Enemies in range take 15% more damage from all sources.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +3% vulnerability bonus | Yes |
| Common | Watchman range +10% | Yes |
| Rare | Marked targets take +1 flat damage from every hit | Yes |
| Rare | Watchman spots enemies earlier: enemies appear highlighted | Repeats slow highlighted enemies |
| Epic | Expose weakness: one random enemy per wave takes 50% more damage | Repeats affect +1 enemy |
| Epic | Warning bell: 3s advance notice before enemies reach castle (slows first arrival) | Repeats add +1s |
| Legendary | All enemies are permanently marked: +15% damage from all sources always | Repeats add +5% |

---

### 16. Enchantress
Grabbed enemies take 5 dmg/sec while held by cursor.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +2 held damage per second | Yes |
| Common | Grabbed enemies glow, revealing nearby hidden enemies | Yes (increases glow radius) |
| Rare | Grabbed enemy is cursed on release: takes +20% damage for 5s | Repeats increase duration |
| Rare | Curse splash: enemies near the grabbed enemy take 3 dmg/sec too | Repeats increase splash range |
| Epic | Soul drain: holding enemies heals castle 1 HP/sec | Repeats increase heal rate |
| Epic | Hex: grabbed enemies shrink, reducing their max HP by 10% | Repeats increase shrink |
| Legendary | Dominate: grabbed enemies fight for you for 3s after release | Repeats add +1s control |

---

### 17. Necromancer
20% chance dead enemies rise as temporary allies for 5 seconds.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +5% resurrection chance | Yes |
| Common | +1s undead ally duration | Yes |
| Rare | Undead allies deal +30% more damage | Yes |
| Rare | Undead allies explode on expiry dealing AoE damage | Repeats increase explosion |
| Epic | Undead allies can resurrect other enemies they kill (chain) | Repeats increase chain chance |
| Epic | Death aura: enemies near undead allies take 3 dmg/sec | Repeats increase damage |
| Legendary | Raise all enemies that die within 3s as undead (mass resurrection) | Repeats increase time window |

---

### 18. Alchemist
Randomly applies fire/ice/lightning/poison to other warriors' attacks.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +5% element application chance | Yes |
| Common | Applied elements are 10% stronger | Yes |
| Rare | Can apply 2 elements simultaneously | Repeats add +1 |
| Rare | Alchemist brews potions: random buff to a warrior every 10s | Repeats reduce interval |
| Epic | Transmutation: enemies killed with elements drop HP orbs (+3 HP) | Repeats increase HP |
| Epic | Volatile mix: enemies with 2+ elements on them take 25% more damage | Repeats increase bonus |
| Legendary | Philosopher's stone: all warriors permanently gain all 4 elements at 50% strength | Repeats increase strength |

---

### 19. Jeweler
Does not attack. Converts one warrior slot into an item slot (4 warrior / 6 item).

The tradeoff: you permanently lose an attacker for an extra item. The Jeweler itself sits in a warrior slot but does not fight -- so you effectively go from 5 attackers to 4, but gain a 6th item.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +3% item effectiveness | Yes |
| Common | +2 max castle HP per item owned | Yes |
| Rare | Items have 10% chance to trigger twice | Repeats increase chance |
| Rare | Gem shield: +1% damage resistance per item owned | Yes |
| Epic | Jeweler fires gem projectiles (weak attack, applies random item effects) | Repeats increase gem damage |
| Legendary | Masterwork: one random item has its effect doubled | Repeats affect +1 item |

---

### 20. War Chief
Does not attack. All warriors deal +15% damage.

A support warrior that amplifies your whole roster. He takes up a slot and doesn't fight, but makes everyone else hit harder.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +3% warrior damage bonus | Yes |
| Common | +3 max castle HP per warrior owned | Yes |
| Rare | Rally: warriors deal +10% damage for 5s after an enemy dies | Repeats increase duration |
| Rare | Fortify: +2% damage resistance per warrior owned | Yes |
| Epic | Battle tactics: warriors coordinate attacks on the same target | Repeats increase coordination bonus |
| Legendary | War cry: every 15s, all warriors fire a bonus attack simultaneously | Repeats reduce interval |

---

## Items (20)

5 base castle slots. Passive effects always active.

---

### 1. Ruby Amulet
All fire damage +30%.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +5% fire damage | Yes |
| Common | Fire effects last 0.5s longer | Yes |
| Rare | Enemies killed by fire explode, dealing fire AoE | Repeats increase explosion |
| Rare | Fire attacks have +10% chance to critically burn (double DoT) | Yes |
| Epic | Inferno: fire patches grow over time instead of shrinking | Repeats increase growth rate |
| Legendary | All warriors gain fire damage (even non-fire warriors) | Repeats increase fire amount |

---

### 2. Sapphire Amulet
Ice slow +20%, slow duration +1s.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +5% slow strength | Yes |
| Common | +0.3s slow duration | Yes |
| Rare | Slowed enemies take 10% more damage | Repeats add +5% |
| Rare | Frozen ground: enemies that stop moving freeze solid for 1s | Repeats increase freeze duration |
| Epic | Blizzard: every 20s, all enemies are slowed for 3s | Repeats reduce interval |
| Legendary | Absolute zero: enemies slowed past 80% freeze and take massive damage | Repeats lower threshold |

---

### 3. Topaz Amulet
Lightning chains to +1 extra target.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | Chain damage +5% | Yes |
| Common | Chain range +10% | Yes |
| Rare | +1 chain target | Yes |
| Rare | Chains briefly stun (0.2s) | Repeats increase stun |
| Epic | Chain lightning can bounce back to already-hit enemies | Repeats increase bounce damage |
| Legendary | Storm: every 20s, lightning strikes all enemies on screen | Repeats reduce interval |

---

### 4. Emerald Amulet
Poison max stacks +3 (8 total).

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +1 max poison stack | Yes |
| Common | Poison tick damage +0.5 | Yes |
| Rare | Poison spreads to adjacent enemies at 50% stacks on death | Repeats increase spread % |
| Rare | Highly poisoned enemies (5+ stacks) move 15% slower | Repeats add +5% slow |
| Epic | Toxic cloud: poisoned enemies leave a trail that poisons others | Repeats increase trail duration |
| Legendary | Plague: poison stacks never expire, persist across the entire level | Repeats increase tick damage |

---

### 5. Opal Amulet
Wind knockback distance doubled.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +15% knockback distance | Yes |
| Common | Knockback also deals 2 damage | Yes (stacks damage) |
| Rare | Knocked enemies collide with others, dealing damage and pushing them too | Repeats increase collision damage |
| Rare | Enemies knocked back move slower for 2s after | Repeats increase slow |
| Epic | Tornado: every 25s, a tornado sweeps across the field pushing all enemies back | Repeats reduce interval |
| Legendary | Enemies knocked off the left edge of the screen are instantly killed | Repeats increase the "edge" zone inward |

---

### 6. Iron Shield Charm
Damage resistance +10%.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +2% damage resistance | Yes |
| Common | +5 max HP | Yes |
| Rare | Reflect 10% of blocked damage back to attacker | Repeats increase reflect |
| Rare | After taking damage, gain +5% resistance for 3s | Repeats increase buff |
| Epic | Damage cap: no single hit can deal more than 15% of max HP | Repeats lower cap |
| Legendary | Fortified: every 4th hit against the castle deals 0 damage | Repeats reduce interval (3rd, then 2nd) |

---

### 7. Healing Totem
Castle regenerates 1 HP/sec.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +0.3 HP/sec regen | Yes |
| Common | +3 max HP | Yes |
| Rare | Burst heal: +10 HP when a level begins | Repeats add +5 |
| Rare | Regen doubles when below 30% HP | Repeats raise threshold by +5% |
| Epic | Overheal: healing can exceed max HP (up to +20 temporary HP) | Repeats increase overheal cap |
| Legendary | Regeneration aura: all warriors attack 20% faster while castle is above 80% HP | Repeats lower threshold |

---

### 8. Stone Skin Amulet
Max HP +30, heal +15 on pickup.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +8 max HP | Yes |
| Common | +3 HP heal | Yes |
| Rare | +5% damage resistance per 50 max HP you have | Yes |
| Rare | Stone armor: first hit each level deals 0 damage | Repeats add +1 free hit |
| Epic | Golem: if HP is above 80%, castle counter-attacks hitting enemies for 10 damage | Repeats increase golem damage |
| Legendary | Unbreakable: castle cannot drop below 1 HP for 5s after first reaching 1 HP (once per level) | Repeats add +2s |

---

### 9. Gravity Gauntlets
Fall damage +40%.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +8% fall damage bonus | Yes |
| Common | Enemies fall 10% faster when released | Yes |
| Rare | Enemies that die from fall damage create a shockwave at impact | Repeats increase shockwave radius |
| Rare | Enemies take +15% damage while airborne from all sources | Yes |
| Epic | Slam: thrown enemies that hit the ground bounce once | Repeats increase bounce height |
| Legendary | Meteor throw: enemies thrown downward explode on impact, massive AoE | Repeats increase explosion |

---

### 10. Giant's Grip
Grab range larger, throw velocity +25%.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +10% throw velocity | Yes |
| Common | +10% grab range | Yes |
| Rare | Can grab enemies slightly earlier (they don't need to be as close) | Yes |
| Rare | Grabbed enemies take 3 damage per second | Yes |
| Epic | Power throw: hold mouse button for 1s to charge a super throw (3x velocity) | Repeats reduce charge time |
| Legendary | Grab 2 enemies at once (closest to cursor) | Repeats add +1 grab |

---

### 11. Crossbow
Replaces throw with click-to-shoot. High damage, 1.5s cooldown. Bolt deals 30 damage.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +3 bolt damage | Yes |
| Common | -0.05s cooldown (min 0.3s) | Yes |
| Rare | Bolts pierce through first enemy | Repeats add +1 pierce |
| Rare | Bolts apply a random element from your warriors | Repeats increase element strength |
| Epic | Explosive bolts: AoE damage on impact (30% of bolt damage) | Repeats increase AoE |
| Legendary | Multishot: fire +1 additional bolt per click (spread pattern) | Yes |

---

### 12. Magnetic Gloves
Enemies drift toward cursor when nearby (small pull radius).

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +10% pull strength | Yes |
| Common | +10% pull radius | Yes |
| Rare | Enemies pulled together collide, dealing damage to each other | Repeats increase collision damage |
| Rare | Pull strength increases the closer enemies get to cursor | Repeats increase scaling |
| Epic | Magnetized enemies take +20% damage from all sources | Repeats increase bonus |
| Legendary | Right-click to repel (push all nearby enemies away with force) | Repeats increase repel force |

---

### 13. Scholar's Tome
XP gain +25%.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +5% XP gain | Yes |
| Common | Enemies give +1 flat bonus XP | Yes |
| Rare | Bonus XP for killing enemies quickly (within 5s of spawning) | Repeats increase bonus |
| Rare | XP gain doubles for the last 3 enemies of a level | Repeats increase enemy count |
| Epic | Knowledge: gain +1 extra gacha choice per pick (pick 1 of 4 instead of 3) | Repeats add +1 choice |
| Legendary | Enlightenment: get 2 gacha picks between levels instead of 1 | Repeats... probably cap at 3 |

---

### 14. Lucky Clover
Better gacha rarity odds (shifts rarity table toward rarer results).

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | Shift +2% from Common to Rare | Yes |
| Common | Shift +1% from Rare to Epic | Yes |
| Rare | Guarantee at least 1 Rare or better in each gacha roll | First gives guarantee, repeats bump to Epic guarantee |
| Rare | Reroll button: discard all 3 choices and get 3 new ones (once per gacha) | Repeats add +1 reroll |
| Epic | Shift +3% from Epic to Legendary | Yes |
| Legendary | One of the 3 gacha choices is always Legendary rarity | Repeats affect +1 additional slot |

---

### 15. Commander's Seal
Converts one item slot into a warrior slot (6 warrior / 4 item).

The inverse of the Jeweler. You lose an item slot but gain a 6th warrior. Good for army-heavy builds.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +3% warrior attack speed | Yes |
| Common | +3 max HP per warrior owned | Yes |
| Rare | Warriors have 10% chance to strike twice | Repeats increase chance |
| Rare | Gain +1% damage resistance per warrior owned | Yes |
| Epic | Synergy bonus: warriors that share an element gain +15% effectiveness | Repeats increase bonus |
| Legendary | Warband: all warriors gain +5% damage for each other warrior you have | Repeats add +2% |

---

### 16. Elemental Prism
+10% element damage per unique element type among your warriors.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +2% per element bonus | Yes |
| Common | Element effects last 10% longer | Yes |
| Rare | Having 3+ elements: all elements deal +15% bonus | Repeats add +5% |
| Rare | Element reactions: fire+ice = steam (blind), fire+poison = explosion, ice+lightning = shatter | Repeats increase reaction damage |
| Epic | Elemental mastery: your strongest element deals +30% damage | Repeats add +10% |
| Legendary | Prismatic burst: every 15s, all enemies are hit with all your elements simultaneously | Repeats reduce interval |

---

### 17. Warlord's Banner
+8% attack speed per warrior you have.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +2% per-warrior attack speed bonus | Yes |
| Common | Warriors deal +1 flat damage | Yes |
| Rare | Warriors gain +3% damage per other warrior | Yes |
| Rare | Rally flag: after killing 5 enemies, warriors go berserk (+25% speed) for 3s | Repeats lower kill threshold |
| Epic | United front: if all warrior slots are full, all warriors deal +30% damage | Repeats add +10% |
| Legendary | Army of one: each warrior counts as 2 for all "per warrior" calculations | Repeats add +1 count |

---

### 18. Vampiric Charm
Heal castle 1 HP per enemy killed.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +0.5 HP per kill | Yes |
| Common | +3 max HP | Yes |
| Rare | Overkill damage is converted to bonus healing | Repeats increase conversion % |
| Rare | Killing 3 enemies within 2s triggers blood rush: +20% warrior attack speed for 3s | Repeats increase duration |
| Epic | Life steal: warriors heal castle for 5% of damage dealt | Repeats add +3% |
| Legendary | Blood pact: castle heals to full HP once per level when it drops below 10% | Repeats raise threshold by +5% |

---

### 19. Chaos Crystal
All warrior attacks have 15% chance to trigger a random element effect.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +3% trigger chance | Yes |
| Common | Random element effects are 10% stronger | Yes |
| Rare | Triggered elements have 25% chance to chain to nearby enemy | Repeats increase chain chance |
| Rare | Chaos proc can trigger 2 elements at once | Repeats add +1 element |
| Epic | Chaos storm: every 20s, all enemies are hit with a random element | Repeats reduce interval |
| Legendary | Chaos cascade: element procs can trigger further procs (chain reaction) | Repeats increase cascade chance |

---

### 20. Battle Standard
Warriors deal +5% damage per enemy currently alive on screen.

**Upgrades:**
| Rarity | Upgrade | Stackable |
|--------|---------|-----------|
| Common | +1% per-enemy damage bonus | Yes |
| Common | +2% warrior attack speed per enemy on screen | Yes |
| Rare | When 5+ enemies are on screen, warriors gain AoE splash (10% dmg) | Repeats increase splash |
| Rare | Underdog: if enemies outnumber warriors 3:1, all damage doubled | Repeats lower ratio |
| Epic | Last stand: the fewer castle HP, the more damage warriors deal (up to +50%) | Repeats increase max bonus |
| Legendary | Overwhelming force: when 8+ enemies on screen, lightning rains from sky dealing AoE | Repeats lower threshold |

---

## Build Path Synergies

### The Pyromancer
**Core:** Fire Archer, Dragon, Bomber
**Boost with:** Ruby Amulet, Blacksmith, Elemental Prism
**Upgrade focus:** Burn spread (Fire Archer Epic), burning ground (Bomber Rare), Ruby's "all warriors gain fire" (Legendary)

### The Frost Controller
**Core:** Ice Archer, Wind Archer
**Boost with:** Sapphire Amulet, Opal Amulet, Gravity Gauntlets
**Upgrade focus:** Freeze solid (Ice Epic), Wind launches enemies airborne (Wind Legendary), fall damage stacking

### Chain Lightning
**Core:** Lightning Archer, Alchemist
**Boost with:** Topaz Amulet, Chaos Crystal, Warlord's Banner
**Upgrade focus:** Max chain targets, stun on chain, Storm legendary procs

### The Throw Master
**Core:** Eagle, Enchantress
**Boost with:** Gravity Gauntlets, Giant's Grip, Magnetic Gloves
**Upgrade focus:** Eagle grab-and-drop (Epic), Enchantress dominate (Legendary), Meteor throw (Legendary)

### The Crossbow Hunter
**Core:** Crossbow (replaces throw), Watchman, Ballista
**Boost with:** Blacksmith, Scholar's Tome
**Upgrade focus:** Multishot (Legendary), explosive bolts, pierce, marked targets take +damage

### The Undying Castle
**Core:** Priestess, Iron Shield, Healing Totem, Stone Skin + Jeweler (4 warrior / 6 item)
**Boost with:** Vampiric Charm, Sapphire Amulet
**Upgrade focus:** Divine shield, damage cap, overheal, blood pact

### The Army General
**Core:** War Chief, Bard, Blacksmith + Commander's Seal (6 warrior / 4 item)
**Boost with:** Warlord's Banner, Battle Standard
**Upgrade focus:** Per-warrior scaling, war cry, warband, united front

### The Plague Doctor
**Core:** Poison Archer, Necromancer, Alchemist
**Boost with:** Emerald Amulet, Chaos Crystal
**Upgrade focus:** Poison never expires (Legendary), toxic cloud spread, undead chain resurrection
