for i in data_sources/*; do \
    sudo curl -X "POST" "http://localhost:3003/api/datasources" \
    -H "Content-Type: application/json" \
     --user admin:admin \
     --data-binary @$i
done

# for i in dashboards/*; do \
#     sudo curl -X "POST" "http://localhost:3003/api/dashboards/db" \
#     -H "Content-Type: application/json" \
#      --user admin:admin \
#      --data-binary @$i
# done