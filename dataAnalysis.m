clear all
data = readtable('data.dat');
dimension = size(data);
numRobots = 0;

for line = 1:dimension(1)
    if data(line,1).(1) == 1
        numRobots = numRobots + 1;
    else
        break
    end
end
robot = [];
error = [];

sample = zeros(1, numRobots);
sumErr = 0;
for line = 1:dimension(1)
     if(data(line,2).(1)==0 && line~=1)
        robot = [robot; sample];
        sample = zeros(1, numRobots);
        error = [error; sumErr];
        sumErr = 0;
     end
    sumErr = sumErr + data(line,4).(1);
    sample(data(line,2).(1)+1) = data(line,3).(1);
end
robot = [robot; sample];
error = [error; sumErr];
step = size(robot);
ti = 1:step(1);

figure(1)
for i = 1:numRobots
    hold on
    plot(ti,robot(:,i), 'LineWidth', 2, 'DisplayName',['robot' num2str(i,'%02d')])
end
title("Target over steps")
ylabel("Target Position")
xlabel("steps")
legend('location','eastoutside')

figure(2)
plot(ti,error, 'LineWidth', 2)
title("Total Error over time")
ylabel("Positional Error")
xlabel("steps")
legend("Error")