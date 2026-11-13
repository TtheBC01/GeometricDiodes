# A plain-language overview of this project

This page is for readers who are new to the topic. It explains what the
project set out to learn, how it went about it, and what it found, with as
little jargon as possible. The technical description of the code is in
[README.md](README.md), and the full write-up is the draft paper in
[paper/main.pdf](paper/main.pdf).

## The question in one sentence

**Can the shape of a tiny graphene channel, on its own, turn terahertz
radiation into usable electricity?**

The answer this project arrives at has two parts.

- **As a detector, sometimes yes, but weakly.** A funnel can turn
  terahertz radiation into a DC signal by its shape, but only if it is very
  short (about half a micrometre for a frequency of one terahertz) and its
  edges are smooth. Even then the shape does no better than a simpler
  effect that comes from how the device is wired.
- **As a power source, no.** Neither shape converts as much as 1 % of the
  incoming power, in any of the cases we studied. The Tesla valve does
  nothing useful in graphene.

## Some background

**Rectennas.** An antenna picks up radiation and turns it into an electric
current that swings back and forth, exactly as fast as the radiation itself
oscillates. To get useful direct current (DC), like the current from a
battery, you need a *rectifier*: a component that lets current pass more
easily in one direction than the other. An antenna joined to a rectifier is
called a *rectenna*. Rectennas already work well for radio waves and
microwaves, for example in wireless charging and contactless cards.

**Why terahertz is hard.** Terahertz radiation sits between microwaves and
infrared light. It oscillates around a trillion times per second. The usual
rectifier, a semiconductor diode, cannot keep up at that speed. So people
have been looking for a rectifier with no diode in it at all.

**Two ideas based on shape.** Graphene is a sheet of carbon one atom thick,
in which electrons move unusually fast and travel a long way before bumping
into anything. That has inspired two proposals in which the *shape* of the
channel is supposed to do the rectifying:

- **The funnel, or "geometric diode".** Electrons are herded through a
  funnel more easily one way than the other, much as a lobster pot lets
  lobsters in but not out.
- **The Tesla valve.** Nikola Tesla patented a pipe with side loops in 1920.
  Fluid passes straight through in one direction, but in the other direction
  the loops turn part of the flow back against itself. It is a one-way valve
  with no moving parts. In very clean graphene, electrons can flow like a
  thick liquid, so an electronic Tesla valve is a natural thing to try.

![The two shapes studied](paper/figs/overview_devices.png)

*The two shapes studied here, drawn to scale. Each device is about one tenth
as long as a human hair is wide. Current enters and leaves through the
coloured edges.*

## What we did

This is a **computer simulation** project. Nothing was built or measured.

Electrons in graphene can behave in three quite different ways, depending on
temperature, device size and how fast you push them. We wrote a separate
simulation for each, and ran the same shapes through all three. For the
shortest funnel we wrote a fourth, more complete simulation of the first
kind (see finding 6):

1. **Like billiard balls.** Electrons fly in straight lines and bounce off
   the walls of the channel.
2. **Like a thick liquid.** Electrons collide with each other so often that
   they flow like honey.
3. **Like waves on that liquid.** At terahertz speeds the electron liquid
   sloshes back and forth, and ripples travel along the channel.

Before using each simulation, we tested it against problems whose exact
answers are known from theory or from published work. There are 64 such
tests in total, and all pass. Where a published measurement exists we
compared against it.

After the first draft we searched the literature for earlier work that
reaches the same conclusions or contradicts them. That search changed the
project: it led to two further sets of simulations, and to a more careful
answer. A third set, which removed the main assumption behind the most
promising result, then cut that result to a sixth. The record of the
search is in [paper/prior_work.md](paper/prior_work.md).

## What we found

**1. The funnel does rectify a steady current, but our model matches the
experiment only under favourable assumptions.** In the billiard-ball
picture, a perfectly clean funnel passes 1.6 times more current one way
than the other. A published experiment on the same shape measured 1.66.
With a realistic amount of scattering inside the graphene the simulated
figure is anywhere between 0.96 and 1.55, depending on where along the
device the applied voltage is assumed to drop. Our model does not calculate
that, so we do not claim to reproduce the measurement.

**2. The Tesla valve does nothing when electrons act like billiard balls.**
Current is the same in both directions, within the noise of the simulation.
The loops only add resistance. A Tesla valve needs a fluid, and bouncing
particles are not one.

**3. When electrons flow like a liquid, the Tesla valve works in principle
but not in practice in graphene.** A Tesla valve only works when the fluid
is moving fast enough for its own momentum to matter. Electrons in graphene
have a speed limit, and they stop behaving like a liquid well before they
reach the speed the valve needs. Inside the range that is physically
allowed, the one-way effect is between 0.1 % and 3 %. Two published
experiments lie outside what our model reproduces. One reports a 5 % effect
in graphene at very small currents, which we cannot explain. The other
reports a very large effect in a different material (gallium arsenide) at
high currents, where the flow is probably turbulent. We did not simulate
that regime.

**4. A control electrode produces a one-way effect of its own, in any
shape.** Real devices sit on a "gate", an electrode that sets how many
electrons are in the graphene. If one end of the device is connected to the
gate, reversing the voltage changes the current by about 1.5 % at 43
millivolts. A plain straight strip does exactly the same. When we instead
swap which end is connected, so that only the shape can matter, less than
0.1 % remains. A measurement that reverses the voltage on a Tesla valve
will therefore see an effect that has nothing to do with the valve.

**5. At terahertz frequencies, a funnel that is a micrometre or more long
produces a DC current because of how it is wired, not because of its
shape.** If one end of the device is fed by the antenna and the other end
is connected to the gate, the device rectifies. That is a known effect, and
a straight strip wired the same way would do it too. When we wire the
funnel symmetrically, so that only its shape can matter, the response
nearly vanishes. The one exception is a narrow range of frequencies where
the taper acts like the horn of a trumpet and helps the wave in. The Tesla
valve contributes ten to twenty times less than the funnel at every
frequency.

**6. A very short funnel does rectify terahertz radiation by its shape,
but only weakly, and only if its edges are smooth.** In the billiard-ball
picture the funnel's one-way behaviour takes time to act, because electrons
need time to cross the device. What matters is how far an electron gets
during one swing of the radiation. The funnel from the published experiment
is 6 micrometres long. It has lost half of its response by 0.06 terahertz
and all of it by 0.3 terahertz. A funnel about half a micrometre long keeps
working at 1 terahertz, at room temperature.

How well it works depends on something our first simulations did not
calculate: how the applied voltage spreads inside the device. They assumed
it, in two different ways, and both gave a strong response, two to eight
times the wiring effect in point 5. We then wrote a simulation that
calculates the voltage from the electrons' own charge, with a gate and
metal contacts included. The response dropped to a sixth: about the same
as the wiring effect at room temperature, and not more. The reason is that
a quarter of the voltage is used up at the two contacts, and the rest
falls evenly on both sides of the narrowest point, which helps the one-way
effect much less than either assumption did.

The same simulation showed two more things. If the edges of the funnel are
rough, so that half of the electrons hitting them bounce off in a random
direction, the effect disappears. And if the gate is close to the graphene
(30 nanometres), a plain straight strip wired as in point 5 gives twice the
signal of the funnel shape. So the length of the device decides whether a
"geometric diode" can work at terahertz frequencies at all, its edges decide
whether it does, and how well a long one works with a steady current says
little about either. One measurement in the literature, at 28 terahertz, is
not reproduced by our model at all. That frequency is beyond what the model
can describe.

**7. As a power source, every version is very inefficient.** At best, less
than 1 % of the incoming terahertz power comes out as DC power. For the
long, wired device we tried changing the two main design choices (how many
electrons are in the graphene, and how close the gate sits). Neither raised
that ceiling. They only changed the power level at which the device stops
improving. Realistic electrical contacts make it several times worse. The
short funnel is far below even that: about one part in ten thousand. That a
ceiling of this kind exists for transistor-like detectors was already known
from earlier work by others. Our contribution is the numbers for graphene.

**8. As detectors, the funnels are respectable.** Detecting a faint signal
is a much less demanding job than harvesting power from it. On paper, the
long wired funnel is about as sensitive as the best graphene terahertz
detectors that have been built and measured at room temperature. The short
funnel, if about twenty of them with smooth edges could be connected to one
antenna without losses, would be about half as sensitive as that. Both are
well short of commercial detectors based on conventional diodes.

## The bottom line

Shape alone can make a graphene terahertz *detector*. It does not make a
power source.

- A **funnel half a micrometre long with smooth edges** rectifies terahertz
  radiation by its shape, about as well as the wiring effect and no
  better. A **long funnel** only does so because of how it is wired, or by
  acting as a horn.
- No funnel we studied is an energy harvester.
- The **Tesla valve** is neither a detector nor a harvester in graphene.

These results tell people working on such devices where the response comes
from and which design choices matter: the length of the device, the wiring,
the distance to the gate, the electrical contacts and the smoothness of the
edges. Adding Tesla loops does not help. They also show that a simulation
which assumes how the voltage spreads inside such a device, as many do,
can overstate the effect several times over.

## What to keep in mind

- **This is simulation, not experiment.** The models are checked against
  theory and against one measured number, but no device was built.
- **The models are idealised, and nearly always in the device's favour.**
  Perfect contacts, a perfect antenna connection and smooth edges were
  assumed unless stated. A real device should do worse. That makes the
  negative conclusions safer, and it makes the positive one, about the
  short funnel, an upper estimate. That estimate has already come down
  once, when an assumption was replaced by a calculation.
- **The short-funnel result has not been checked against an experiment.**
  The simulation that calculates the voltage still treats the contacts as
  ideal, and a quarter of the voltage is lost there, so a different
  contact could change the answer. We tried only perfectly smooth and
  half-rough edges, so we do not know how smooth the edges need to be.
- **Some physics is left out.** In particular, real graphene detectors also
  respond because the radiation heats the electrons. That effect is not in
  our models. A mechanism outside the three we studied could behave
  differently.
- **The paper is a draft.** It has not been peer reviewed. This is its
  third version. The first concluded that shape alone does not work at
  all. The second, after the short-funnel simulations, concluded that a
  short funnel works well. The third, after the voltage inside the funnel
  was calculated, is in between: it works, weakly, under conditions.

## Where to look next

| If you want | Go to |
|---|---|
| The full argument with figures | [paper/main.pdf](paper/main.pdf) |
| The tests each simulation had to pass | [paper/supplement.pdf](paper/supplement.pdf) |
| Detailed results, stage by stage | the `results/` folder, starting with [results/KILLTEST.md](results/KILLTEST.md) |
| The short funnel with an assumed voltage profile | [results/BALLISTIC_SHORT.md](results/BALLISTIC_SHORT.md) |
| The short funnel with the voltage calculated | [results/BALLISTIC_SELFCONSISTENT.md](results/BALLISTIC_SELFCONSISTENT.md) |
| The effect of the gate on steady currents | [results/HYDRO_GATE.md](results/HYDRO_GATE.md) |
| What earlier work by others already showed | [paper/prior_work.md](paper/prior_work.md) |
| How to build and run the code | [README.md](README.md) |

## A few terms

- **Graphene:** a sheet of carbon atoms one atom thick. Electrons move
  through it very fast and with little scattering.
- **Terahertz (THz):** radiation between microwaves and infrared light,
  oscillating about a trillion times per second.
- **Rectifier:** a component that passes current more easily one way than
  the other, turning a back-and-forth current into a one-way current.
- **Rectenna:** an antenna joined to a rectifier, used to turn radiation
  into DC electricity.
- **Detector versus harvester:** a detector only has to notice that
  radiation is there. A harvester has to convert a useful fraction of its
  power. The second is far harder.
