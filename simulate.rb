#!/usr/bin/ruby

def valid_float?(param='no float')


	!!Float(param) rescue false
end


def mean(array)
  array.inject(0) { |sum, x| sum += x } / array.size.to_f
end
def standard_deviation(array)
  m = mean(array)
  variance = array.inject(0) { |variance, x| variance += (x - m) ** 2 }
  return Math.sqrt(variance/(array.size-1))
end



meanONDuration = ARGV[2]
mode = ARGV[1]
ber = ARGV[0]

config = [1, 5, 10, 15]



config.each do |nSF|

	mptcp_results = []
	tcp_results = []
	duration = 30

	

	while duration < 481 do


		if mode == 4.to_s
			log_i=0
			while log_i < 4 do
				remove_command = ('rm log' << log_i.to_s << 'ber' << ber << 'meanON' << meanONDuration<<'mode'<<mode << '.txt')
				system(remove_command)
				log_i+=1
			end
		else
			log_i=0
			while log_i < 4 do
				remove_command = ('rm log' << log_i.to_s << 'ber' << ber<<'mode'<<mode << '.txt')
				system(remove_command)
				log_i+=1
			end
		end


		


		if mode == 1.to_s || mode == 3.to_s

			cmd='(NS_GLOBAL_VALUE="RngRun=4" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --ber=' << ber  <<' --mode='<< mode <<'")2>log0ber' << ber<<'mode'<<mode  << '.txt & '
		    cmd << '(NS_GLOBAL_VALUE="RngRun=5" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --ber=' << ber  <<' --mode='<< mode  <<'")2>log1ber' << ber<<'mode'<<mode  <<'.txt & '
            cmd << '(NS_GLOBAL_VALUE="RngRun=6" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --ber=' << ber  <<' --mode='<< mode  <<'")2>log2ber' << ber<<'mode'<<mode  <<'.txt & '
		    cmd << '(NS_GLOBAL_VALUE="RngRun=7" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --ber=' << ber  <<' --mode='<< mode  <<'")2>log3ber' << ber<<'mode'<<mode  <<'.txt &
		    wait'
		elsif mode == 4.to_s
			cmd='(NS_GLOBAL_VALUE="RngRun=0" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --per=' << ber  <<' --mode='<< 2.to_s << ' --meanONDuration=' << meanONDuration  <<'")2>log0ber' << ber << 'meanON' << meanONDuration<<'mode'<<mode  <<'.txt & '
		    cmd << '(NS_GLOBAL_VALUE="RngRun=1" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --per=' << ber  <<' --mode='<< 2.to_s << ' --meanONDuration=' << meanONDuration  <<'")2>log1ber' << ber << 'meanON' << meanONDuration<<'mode'<<mode  <<'.txt & '
            cmd << '(NS_GLOBAL_VALUE="RngRun=2" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --per=' << ber  <<' --mode='<< 2.to_s << ' --meanONDuration=' << meanONDuration  <<'")2>log2ber' << ber << 'meanON' << meanONDuration<<'mode'<<mode  <<'.txt & '
		    cmd << '(NS_GLOBAL_VALUE="RngRun=3" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --per=' << ber  <<' --mode='<< 2.to_s << ' --meanONDuration=' << meanONDuration  <<'")2>log3ber' << ber << 'meanON' << meanONDuration<<'mode'<<mode  <<'.txt &
		    wait'
		else
			cmd='(NS_GLOBAL_VALUE="RngRun=0" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --per=' << ber  <<' --mode='<< mode  <<'")2>log0ber' << ber<<'mode'<<mode  <<'.txt & '
		    cmd << '(NS_GLOBAL_VALUE="RngRun=1" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --per=' << ber  <<' --mode='<< mode  <<'")2>log1ber' << ber<<'mode'<<mode  <<'.txt & '
            cmd << '(NS_GLOBAL_VALUE="RngRun=2" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --per=' << ber  <<' --mode='<< mode  <<'")2>log2ber' << ber<<'mode'<<mode  <<'.txt & '
		    cmd << '(NS_GLOBAL_VALUE="RngRun=3" ./waf --run "SatSim --nSF=' << nSF.to_s  << ' --duration=' << duration.to_s  << ' --per=' << ber  <<' --mode='<< mode  <<'")2>log3ber' << ber<<'mode'<<mode  <<'.txt &
		    wait'
		end

		system(cmd)

		if mode == 4.to_s
			i = 0
			while i < 4 do
				filename = 'log'<<i.to_s<<'ber'<<ber << 'meanON' << meanONDuration<<'mode'<<mode <<'.txt'
				first_line = ((File.open(filename).to_a).last rescue puts 'no file here')

				splitLine = first_line.split("\t")

				if valid_float?(splitLine[0])
					mptcp_results << splitLine[0].to_f
				end
				if valid_float?(splitLine[1])
                	tcp_results << splitLine[1].to_f
                end


				i+=1
			end
		else
			i = 0
			while i < 4 do
				filename = 'log'<<i.to_s<<'ber'<<ber<<'mode'<<mode<<'.txt'
				first_line = ((File.open(filename).to_a).last rescue puts 'no file here')

				splitLine = first_line.split("\t")

                if valid_float?(splitLine[0])
          			mptcp_results << splitLine[0].to_f
                end
                if valid_float?(splitLine[1])
                    tcp_results << splitLine[1].to_f
                end


				i+=1
			end 
		end

		duration+=30
	end


    # MPTCP AUSWERTUNG

	total=0
	counter = 0
	for r in mptcp_results do

		total+=r

		counter += 1
	end

	avg='no data'
	stdev = ''

	avg = total/counter unless counter == 0

	if !(avg == 'no data')
		stdev = standard_deviation mptcp_results
	end

	avg = (avg.to_s + '    ' + stdev.to_s)

	output_name = ""

	if mode == 4.to_s
		output_name = ('MPTCP_mode=' << mode << '_ber=' << ber << '_nSF=' << nSF.to_s << '_meanOnDuration=' << meanONDuration << '.txt' )
	else
		output_name = ('MPTCP_mode=' << mode << '_ber=' << ber << '_nSF=' << nSF.to_s << '.txt' )
	end

	File.open(output_name, 'w'){|f| f.write(avg)}

	#TCP AUSWERTUNG

	tcp_total=0
    tcp_counter = 0
    for r in tcp_results do

    	tcp_total+=r

    	tcp_counter += 1
    end

    avg='no data'
    stdev = ''

    avg = tcp_total/tcp_counter unless tcp_counter == 0

    if !(avg == 'no data')
    	stdev = standard_deviation tcp_results
    end

    avg = (avg.to_s + '    ' + stdev.to_s)

    output_name = ""

    if mode == 4.to_s
    	output_name = ('TCP_mode=' << mode << '_ber=' << ber << '_nSF=' << nSF.to_s << '_meanOnDuration=' << meanONDuration << '.txt' )
    else
    	output_name = ('TCP_mode=' << mode << '_ber=' << ber << '_nSF=' << nSF.to_s << '.txt' )
    end

    File.open(output_name, 'w'){|f| f.write(avg)}
	
	
end



