# doubles
Calculating a the average of a list of doubles is seems deceptively simple. When working with real numbers, `Avg = sum/ num_elem`.  However because the nature of the IEEE floating point format, is essentially base-2 scientific
notation with a limited number of precesion digits, error can accumulate when representing the sum requires more precision digits than those represented by the floating point representation.

Let's use a base-10 anology calculator to explore this problem.

Suppose you want to build a calculator that can represent numbers upto 5 digits long. 

If you want to do math with whole numbers, no probelem.
So 45231 = 40000 + 5000 + 200 + 30 + 1 = 4 * 10<sup>4</sup> + 5 * 10<sup>3<\sup> + 2 * 10<sup>2</sup> + 3 * 10<sup>1<\sup> + 1 * 10<sup>0<\sup>
You now can represent numbers from 0 to 9,999! Pretty good.

How about representing fractional numbers? Or approximating Irrational numbers? Or any other real number? What about HUGE numbers?
How do you represent those numbers?

The solution I present next is a simplification of how IEEE decided to represent floating point. (The sign bit, the implied leading 1, or denormalized numbers are not critical to understanding solution to my doubles avg solution)

Say you design your calculator such that for these fractional numbers, the numbers are broken up as such:

  `EE DDD`

Here E represents exponent bits, and D represents digit bits. The exponent can represent values from 0 to (10<sup>2</sup> - 1), and the digit numbers (10<sup>3</sup> - 1).
Now you can represent numbers MUCH bigger than the 9,999 maximum. Okay great, but what about really really small numbers?
  
Now 45231(F) represents 2.31 * 10<sup>45<\sup>. That's 43 0s!
  
You decide to sacrifice your ability to represent the biggest numbers so that you can also represent the smallest. You say, the number EEE is not the true exponent. You want to devide the exponent values so that half represent the really big and half the really small.
The new exponent e is now calculated, e = EE - 50.

Now 45231(F) represents 2.31 * 10<sup>-5<\sup> = 0.0000231. Very small!
  
 2.31 * 10<sup>45<\sup> is now represented by 95231(F).
  
  
My solution is to store several sums grouped by multiples of exponent.
