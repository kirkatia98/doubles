# doubles
# Abstract

Calculating a the average of a list of doubles is seems deceptively simple.
When working with real numbers, `Avg = sum/num_elem`.
However because the nature of the IEEE floating point format, is essentially base-2 scientific notation with a limited number of precesion digits, error can accumulate when representing the sum requires more precision digits than those represented by the floating point representation. To get around this I represent the exact sum by adding mantissas for doubles of similar magnitude into one of 512 + 16 buckets grouped by exponent, overflowing if needed. Then I individually average each bucket, and 
percolate the remainder down until only the smallest bucket holds value. This allows for a error rate of 0 in most cases.

# ELI5 Explanation
Let's use a base-10 anology calculator to explore this problem.
Suppose you want to build a calculator that can represent numbers upto 5 digits long. 

If you want to do math with whole numbers, ei integers, no probelem.

So 45231 = 40000 + 5000 + 200 + 30 + 1 = 4 * 10<sup>4</sup> + 5 * 10<sup>3</sup> + 2 * 10<sup>2</sup> + 3 * 10<sup>1</sup> + 1 * 10<sup>0</sup>

You now can represent numbers from 0 to 9,999! Pretty good.
How about representing fractional numbers? Or approximating Irrational numbers? Or any other real number? What about HUGE numbers?
How do you represent those numbers?

*The solution I present next is a simplification of how IEEE decided to represent floating point. (The sign bit, the implied leading 1, or denormalized numbers are not critical to understanding solution to my doubles avg solution).*

Say you design your calculator such that for these fractional numbers, the numbers are broken up as such:

  `EE DDD`

Here `E` represents exponent bits, and `D` represents digit bits. 
The exponent can represent values from 0 to (10<sup>2</sup> - 1), and the digit numbers (10<sup>3</sup> - 1).
Now you can represent numbers MUCH bigger than the 9,999 maximum.
  
Now 45231(F) represents 2.31 * 10<sup>45</sup>. That's 43 0s!

Okay great, but what about really really small numbers?

<br>
You decide to sacrifice your ability to represent the biggest numbers so that you can also represent the smallest. You say, the number `EE` is not the true exponent. You want to devide the exponent values so that half represent the really big and half the really small.

The new exponent e is now calculated, `e` = `EE` - 49. 49 = 10<sup>2</sup>/2 - 1. So exponents 0-49 are for numbers <= 1, and 50-99, are for numbers > 1.

45231(F) represents 2.31 * 10<sup>-4</sup> = 0.000231. Very small!
  
2.31 * 10<sup>45</sup> is now represented by 94231(F).
 
<br>
Cool. So now we can represent the very big and the very small, and we can approximate numbers like 1/3 as 3.33 * 10<sup>-1</sup> = 48333(F).
What to call your representation? How about... 'Floating point', or 'floats' for short. Because the decimal point can "float" around based on the exponent value.

In a couple years, due to harware advancements, you are able to design and build a NEW calculator, with 10 digits instead of just 5. Having just 2 digits for the exponent field no longer makes sense, so you make a new format, `EEE DDDDDDD`, and call this new format 'Double'. It has more range and more precision!

<br>
Now back to the original problem, average of a list of doubles. (For simplicity, let's stick with floats). The naive solution would be to add all the numbers then average them.

### Here's a simple example:

49100(F) = 1.00 <br>
49200(F) = 2.00 <br>
49500(F) = 5.00 <br>

Adding them together, you get: 49800(F) = 8.00

And dividing by 3: 49800(F) = 2.67, which is 2 2/3 or 2.6666.... rounded. No problem here.

### But suppose you get:

99100(F) = 1.00 * 10<sup>50</sup> <br>
49100(F) = 1.00 <br>
00100(F) = 1.00 * 10<sup>-49</sup> <br>

The real value of the sum would be: 1..000000...1 ....0000...1

UH OH. You need 100 digits of precision to even represent this value. Luckily, when you present the final solution, the error is outside the represented precision. 

### What if you get:

49100(F) = 9.00 <br>
46499(F) = 0.00499 ... 249 times. That's sums to a total of 1.24251

Problem: Each time you add 9.00 and 0.00499, you get 9.00499, which then gets rounded to 2 digits of precision to get... 9.00. Hmmm. Not good.
The real value of the sum would be 10.24251, but at the end the sum you calculate naively is still just 9.00. 

Your average, should be 10.24251/250 = 0.04, but the computed average is 9.00/250 = 0.03
In this handpicked example, the error rate is 25%!!!

<br>
Okay. So what can be done? This is where I explain crux of my solution using the base-10 calculator analogy. The insight is to sum values of similar magnitude together.

Let's split the exponent values into groups of 5, for a total of 20 groups. 0-4, 5-9, 10-14...95-99. Buckets are integers values, and the `DDD` part of the float is treated as an integer. The value is summed (poured?) into the appropriate bucket based on it exponent. When a bucket overflows, the value stored in it devided (concentrated?) and transfered to the next bucket up. Like an abbacus! Now you can use 20 integers of 10 digits each to accurately represent the true sum of any
number of digits.


Once you've summed all the values into buckets, it's time to compute the average. Starting with highest exponent bucket, compute the average for bucket, and percolatte the remainder down to the next bucket by multiplying(dilluting) the value. At the end of this process, you'll have 20 integer values that when summed represent the true average, as well as a remainder.

These avg values can be added up together after being scaled with the apporiate exponent, much like digits 4, 5, 2, 3, and 1 were added as 40000, 5000, 200, 30, and 1 in the very first example. By starting from the smallest bucket, and "snowballing" a sum upto the highest bucket, no error is accumulated that would be displayed within the range of precision.

In where the resulting sum is so small as to be comprable in number of elements, an error can still result because error during the division.






