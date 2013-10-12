Random Test – an Idea

Do you want your application to work fast, have no bugs and be no resource hog
for your environment? Sure, everyone wants. You are testing it thoroughly since
the very beginning to achieve that.

But: manual integration tests are expensive, unit developers tests are hard to
implement properly and your latest click’n'play tool requires more and more
maintenance effort for failing test cases along development?

What if we forget for a moment about scripting your UI by static scripts and
replace it with totally random input? You will get for free coverage in almost
whole application. “Wait” – you will say -  “but there’s no way to check
results as the input is random, isn’t there?”.

Sure, here Design By Contract + Continuous Integration comes to play. You embed
in your system assertions in any portion of code. Failed assertions do not
crash application, but are reported immediately to central server and
aggregated into reports every day. Crashes, warnings, errors are reported, too.
Then you can measure quality of your system-under-test by observing changes in
number of errors, day by day. No scripting required to cover any new line of
code! It will be tested automatically!

Dariusz Cieslak, randomtest.net

