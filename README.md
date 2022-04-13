# doubles
Calculating a the average of a list of doubles is seems deceptively simple. When working with real numbers, `Avg = sum/ num_elem`.  However because the nature of the IEEE floating point format, is essentially base-2 scientific
notation with a limited number of precesion digits, error can accumulate when representing the sum requires more precision digits than those represented by the floating point representation.

Let's use a base-10 anology calculator to explore this problem.

Suppose you want to build a calculator that can represent numbers upto 5 digits long. 

If you want to do math with whole numbers, no probelem.
So 45231 = 40000 + 5000 + 200 + 30 + 1 = 4 * 10<sup>4</sup> + 5 * 10<sup>3</sup> + 2 * 10<sup>2</sup> + 3 * 10<sup>1</sup> + 1 * 10<sup>0</sup>
You now can represent numbers from 0 to 9,999! Pretty good.

How about representing fractional numbers? Or approximating Irrational numbers? Or any other real number? What about HUGE numbers?
How do you represent those numbers?

The solution I present next is a simplification of how IEEE decided to represent floating point. (The sign bit, the implied leading 1, or denormalized numbers are not critical to understanding solution to my doubles avg solution)

Say you design your calculator such that for these fractional numbers, the numbers are broken up as such:

  `EE DDD`

Here `E` represents exponent bits, and `D` represents digit bits. The exponent can represent values from 0 to (10<sup>2</sup> - 1), and the digit numbers (10<sup>3</sup> - 1).
Now you can represent numbers MUCH bigger than the 9,999 maximum. Okay great, but what about really really small numbers?
  
Now 45231(F) represents 2.31 * 10<sup>45</sup>. That's 43 0s!
  
You decide to sacrifice your ability to represent the biggest numbers so that you can also represent the smallest. You say, the number `EE` is not the true exponent. You want to devide the exponent values so that half represent the really big and half the really small.
The new exponent e is now calculated, e = `EE` - 49. 49 = 10<sup>2</sup>/2 - 1. So exponents 0-49 are for numbers <= 1, and 50-99, are for numbers > 1.

Now 45231(F) represents 2.31 * 10<sup>-4</sup> = 0.000231. Very small!
  
2.31 * 10<sup>45</sup> is now represented by 94231(F).
 

Cool. So now we can represent the very big and the very small, and we can approximate numbers like 1/3 as 3.33 * 10<sup>-1</sup> = 48333(F).
What to call your representation? How about... 'Floating point', or 'floats' for short. because the decimal point can "float" around based on the exponent value. In a couple years, due to
harware advancements, you are able to design and build a NEW calculator, with 10 digits instead of just 5. Having just 2 digits for the exponent field no longer makes sense, so you make a new format, `EEE DDDDDDD`, and call this new format 'Double'. It has more range and more precision!


Now back to the original probelem, average of a list of doubles. (For simplicity, let's stick with floats.) The naive solution would be to add all the numbers then average 
them.

49100(F) = 1.00
49200(F) = 2.00
49500(F) = 5.00

Adding them together, you get:
49800(F) = 8.00,
And dividing by 3:
49800(F) = 2.67, which is 2 2/3 or 2.6666.... rounded. No problem here.

But suppose you get:

99100(F) = 1.00 * 10<sup>50</sup>
49100(F) = 1.00
00100(F) = 1.00 * 10<sup>-49</sup>

The real value of the sum would be: 1..000000...1 ....0000...1

UH OH. You need 100 digits of precision to even represent this value. Luckily, when you present the final solution, the error is outside the represented precision. 

What if you get:
49100(F) = 9.00
46499(F) = 0.00499 ... 249 times. That's sums to a total of 1.24251

Problem: Each time you add 9.00 and 0.00499, you get 9.00499, which then gets rounded to 2 digits of precision to get... 9.00. Hmmm. Not good.

The real value of the sum would be 10.24251, but at the end the sum you calculate naively is still just 9.00. 

Your average, should be 10.24251/250 = 0.04, but the computed average is 9.00/250 = 0.03

In this handpicked example, the error rate is 25%!!!

 





