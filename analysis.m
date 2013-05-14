reno = importdata('reno.csv')
tahoe = importdata('tahoe.csv')
std = importdata('std.csv')
newReno = importdata('newReno.csv')

figure(1);
subplot(221)
plot(std(:,1), std(:,2));
title('Standard TCP');
xlabel('Time [Seconds]')
ylabel('CWND Size');
subplot(222)
plot(tahoe(:,1), tahoe(:,2),'r');
xlabel('Time [Seconds]')
ylabel('CWND Size');
title('TCP Tahoe');
subplot(223)
plot(reno(:,1), reno(:,2),'m')
xlabel('Time [Seconds]')
title('TCP Reno');
ylabel('CWND Size');
subplot(224)
plot(newReno(:,1), newReno(:,2),'c')
xlabel('Time [Seconds]')
title('TCP New Reno');
ylabel('CWND Size');
